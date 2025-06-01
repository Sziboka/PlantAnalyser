FROM python:3.11-slim

WORKDIR /app

COPY src/webserver.py .

RUN pip install --no-cache-dir flask pymongo
EXPOSE 9090


CMD ["python", "webserver.py"]