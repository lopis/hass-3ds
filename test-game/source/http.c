#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>

#include <3ds.h>

char *TOKEN = "eyJhbGciOiJIUzI1NiIsInR5cCI6IkpXVCJ9.eyJpc3MiOiJkZmM2MzI4YzYxYjM0MGNmOWIzMDUwMjY4ZWJmZDExYiIsImlhdCI6MTcwMTU1MzEzMywiZXhwIjoyMDE2OTEzMTMzfQ.4AcV3hG7yR3JyAOgj5ONbf7RNcKTtAz74JZucaJiTqg";

char *concat(const char *s1, const char *s2)
{
  char *result = malloc(strlen(s1) + strlen(s2) + 1); // +1 for the null-terminator
  // in real code you would check for errors in malloc here
  strcpy(result, s1);
  strcat(result, s2);
  return result;
}

Result http_post(const char *url, const char *data)
{
  Result ret = 0;
  httpcContext context;
  char *newurl = NULL;
  u32 statuscode = 0;
  u32 contentsize = 0, readsize = 0, size = 0;
  u8 *buf, *lastbuf;

  // printf("POSTing %s\n", url);

  do
  {
    ret = httpcOpenContext(&context, HTTPC_METHOD_POST, url, 0);
    // printf("return from httpcOpenContext: %" PRIx32 "\n", ret);

    // This disables SSL cert verification, so https:// will be usable
    ret = httpcSetSSLOpt(&context, SSLCOPT_DisableVerify);
    // printf("return from httpcSetSSLOpt: %" PRIx32 "\n", ret);

    // Enable Keep-Alive connections
    ret = httpcSetKeepAlive(&context, HTTPC_KEEPALIVE_ENABLED);
    // printf("return from httpcSetKeepAlive: %" PRIx32 "\n", ret);

    char *authorization = concat("Bearer ", TOKEN);
    ret = httpcAddRequestHeaderField(&context, "Authorization", authorization);

    // Set a User-Agent header so websites can identify your application
    ret = httpcAddRequestHeaderField(&context, "User-Agent", "Nintendo 3DS/Home Assistant 3DS");
    // printf("return from httpcAddRequestHeaderField: %" PRIx32 "\n", ret);

    // Set a Content-Type header so websites can identify the format of our raw body data.
    // If you want to send form data in your request, use:
    // ret = httpcAddRequestHeaderField(&context, "Content-Type", "multipart/form-data");
    // If you want to send raw JSON data in your request, use:
    ret = httpcAddRequestHeaderField(&context, "Content-Type", "application/json");
    // printf("return from httpcAddRequestHeaderField: %" PRIx32 "\n", ret);

    // Post specified data.
    // If you want to add a form field to your request, use:
    // ret = httpcAddPostDataAscii(&context, "data", value);
    // If you want to add a form field containing binary data to your request, use:
    // ret = httpcAddPostDataBinary(&context, "field name", yourBinaryData, length);
    // If you want to add raw data to your request, use:
    ret = httpcAddPostDataRaw(&context, (u32 *)data, strlen(data));
    // printf("return from httpcAddPostDataRaw: %" PRIx32 "\n", ret);

    ret = httpcBeginRequest(&context);
    if (ret != 0)
    {
      httpcCloseContext(&context);
      if (newurl != NULL)
        free(newurl);
      return ret;
    }

    ret = httpcGetResponseStatusCode(&context, &statuscode);
    if (ret != 0)
    {
      httpcCloseContext(&context);
      if (newurl != NULL)
        free(newurl);
      return ret;
    }

    if ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308))
    {
      if (newurl == NULL)
        newurl = malloc(0x1000); // One 4K page for new URL
      if (newurl == NULL)
      {
        httpcCloseContext(&context);
        return -1;
      }
      ret = httpcGetResponseHeader(&context, "Location", newurl, 0x1000);
      url = newurl; // Change pointer to the url that we just learned
      // printf("redirecting to url: %s\n", url);
      httpcCloseContext(&context); // Close this context before we try the next
    }
  } while ((statuscode >= 301 && statuscode <= 303) || (statuscode >= 307 && statuscode <= 308));

  if (statuscode != 200)
  {
    // printf("URL returned status: %" PRIx32 "\n", statuscode);
    httpcCloseContext(&context);
    if (newurl != NULL)
      free(newurl);
    return -2;
  }

  // This relies on an optional Content-Length header and may be 0
  ret = httpcGetDownloadSizeState(&context, NULL, &contentsize);
  if (ret != 0)
  {
    httpcCloseContext(&context);
    if (newurl != NULL)
      free(newurl);
    return ret;
  }

  // printf("reported size: %" PRIx32 "\n", contentsize);

  // Start with a single page buffer
  buf = (u8 *)malloc(0x1000);
  if (buf == NULL)
  {
    httpcCloseContext(&context);
    if (newurl != NULL)
      free(newurl);
    return -1;
  }

  do
  {
    // This download loop resizes the buffer as data is read.
    ret = httpcDownloadData(&context, buf + size, 0x1000, &readsize);
    size += readsize;
    if (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING)
    {
      lastbuf = buf; // Save the old pointer, in case realloc() fails.
      buf = realloc(buf, size + 0x1000);
      if (buf == NULL)
      {
        httpcCloseContext(&context);
        free(lastbuf);
        if (newurl != NULL)
          free(newurl);
        return -1;
      }
    }
  } while (ret == (s32)HTTPC_RESULTCODE_DOWNLOADPENDING);

  if (ret != 0)
  {
    httpcCloseContext(&context);
    if (newurl != NULL)
      free(newurl);
    free(buf);
    return -1;
  }

  // Resize the buffer back down to our actual final size
  lastbuf = buf;
  buf = realloc(buf, size);
  if (buf == NULL)
  { // realloc() failed.
    httpcCloseContext(&context);
    free(lastbuf);
    if (newurl != NULL)
      free(newurl);
    return -1;
  }

  // printf("response size: %" PRIx32 "\n", size);

  // Print result
  // printf((char *)buf);
  // printf("\n");

  gfxFlushBuffers();
  gfxSwapBuffers();

  httpcCloseContext(&context);
  free(buf);
  if (newurl != NULL)
    free(newurl);

  return 0;
}

void toggleLights()
{
  Result ret = 0;
  httpcInit(4 * 1024 * 1024); // Buffer size when POST/PUT.
  char *url = "http://192.168.178.39:8123/api/services/light/toggle";
  return http_post(url, "{\"entity_id\": \"light.bedroom\"}");
}
