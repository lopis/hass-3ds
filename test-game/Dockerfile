# Use a base image with DevkitARM installed
FROM devkitpro/devkitarm

# Set the working directory
WORKDIR /usr/src/app

# Copy your source files into the container
COPY . .

# Build your project
WORKDIR /usr/src/app
RUN make