import logging
from flask import Flask, request, jsonify
from pymongo import MongoClient
import smtplib
from email.mime.text import MIMEText

# Configure logging
logging.basicConfig(
    level=logging.INFO,
    format='[%(asctime)s] %(levelname)s in %(module)s: %(message)s'
)

app = Flask(__name__)

# MongoDB connection (use 'mongo' as hostname in Docker Compose network)
client = MongoClient("mongodb://root:example@mongo:27017/")
db = client["plantDB"]
weather_collection = db["weather"]

def send_alert_email(moisture_value):
    msg = MIMEText(f"Alert: Time to Water!\n the moisture value is high: ({moisture_value})")
    msg['Subject'] = 'PlantAnalyser Moisture Alert🌱'
    msg['From'] = 'xxx'
    msg['To'] = 'xxx'
    try:
        with smtplib.SMTP('smtp.gmail.com', 587) as server:
            server.starttls()
            # Login required for Gmail SMTP
            server.login('xxx', 'xxx')
            server.send_message(msg)
        app.logger.info("Alert email sent.")
    except Exception as e:
        app.logger.error(f"Failed to send alert email: {e}")

isWet = True 

@app.route('/', methods=['POST'])
def handle_post():
    global isWet
    data = request.get_json(force=True)
    app.logger.info(f"Received POST data: {data}")
    app.logger.info(f"Headers: {dict(request.headers)}")
    # Insert data into MongoDB
    result = weather_collection.insert_one(data)
    app.logger.info(f"Inserted document with _id: {result.inserted_id}")

    # Check moisture and send email if needed
    if 'moisture' in data and data['moisture'] >= 1488:
        if isWet:
            send_alert_email(data['moisture'])
            isWet=False
    else:
        isWet=True
    return 'OK', 200

@app.route('/', methods=['GET'])
def handle_get():
    app.logger.info("Received GET request")
    app.logger.info(f"Headers: {dict(request.headers)}")
    return 'Hello, World!', 200

@app.route('/weather', methods=['GET'])
def get_weather():
    docs = list(weather_collection.find({}, {'_id': 0}))
    # Return a flat array, not wrapped in a "weather" key
    return jsonify(docs), 200

if __name__ == '__main__':
    app.run(host='0.0.0.0', port=9090)