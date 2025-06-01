from pymongo import MongoClient
from datetime import datetime, timedelta, timezone
import random

client = MongoClient("mongodb://root:example@localhost:27017/")
db = client["plantDB"]
weather_collection = db["weather"]

base_time = datetime.now(timezone(timedelta(hours=2))) - timedelta(days=1)
docs = []

for i in range(20):
    doc_time = base_time + timedelta(minutes=i*30)
    doc = {
        "humidity": round(random.uniform(45, 55), 2),
        "temp_c": round(random.uniform(24, 28), 2),
        "temp_f": round(random.uniform(75, 83), 2),
        "heat_index_c": round(random.uniform(25, 29), 2),
        "heat_index_f": round(random.uniform(77, 85), 2),
        "timestamp": doc_time.isoformat()  # This gives 'YYYY-MM-DDTHH:MM:SS+02:00'
    }
    docs.append(doc)

weather_collection.insert_many(docs)
print("Inserted 20 fake weather documents.")