# Use official Python image
FROM python:3.11-slim

# Set working directory
WORKDIR /app

# Copy webserver code
COPY src/webserver.py .

# Install Flask
RUN pip install --no-cache-dir flask

# Expose port
EXPOSE 9090

# Run the webserver
CMD ["python", "webserver.py"]