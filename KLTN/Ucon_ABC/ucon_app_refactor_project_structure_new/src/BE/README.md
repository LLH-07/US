# ucon-ebook-demo
run server:
UCON_TRIGGER_POLLER_INTERVAL_MS=250 \
UCON_TIMER_SCHEDULER_INTERVAL_MS=1000 \
mvn spring-boot:run

run client web socket: npx wscat -c "ws://localhost:8081/ws/pep/control?userId=1"
-> receive connectionId that uses for read try api

run test api:
1. read try: 
curl -X POST \
  "http://localhost:8081/api/pep/read/try?userId=1&volumeId=2001" \
  -H "X-Control-Connection-Id: <connection-id>"

  curl -X POST "http://localhost:8081/api/pep/read/try?userId=1&volumeId=7001&accessPath=FAMILY_SHARED" \
  -H "X-Control-Connection-Id: <connection-id>"

2. get chunk:
curl -i "http://localhost:8081/api/pep/read/sessions/1/chunks/1"

3. end session:
curl -X POST "http://localhost:8081/api/pep/read/sessions/1/end"

4. fulfill obligation:
curl -X POST \
"http://localhost:8081/api/ucon/obligations/EBOOK_TERMS_V1/fulfill" \
-H "Content-Type: application/json" \
-d '{
  "userId": 1,
  "policyRef": "rental_read_pre_policy",
  "evidence": {
    "accepted": true
  }
}'
