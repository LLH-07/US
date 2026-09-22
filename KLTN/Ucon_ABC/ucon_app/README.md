# ucon-ebook-demo
run server: mvn spring-boot:run

run client web socket: npx wscat -c "ws://localhost:8081/ws/pep/control?userId=1"
-> receive connectionId that uses for read try api

run test api:
1. read try: 
curl -X POST \
  -H "X-Control-Connection-Id: <connection-id>" \
  "http://localhost:8081/api/pep/read/try?userId=1&volumeId=2"

  curl -X POST "http://localhost:8081/api/pep/read/try?userId=1&volumeId=7001&accessPath=FAMILY_SHARED" \
  -H "X-Control-Connection-Id: <connection-id>"

2. get chunk:
curl -i "http://localhost:8081/api/pep/read/sessions/1/chunks/1"

3. end session:
curl -X POST "http://localhost:8081/api/pep/read/sessions/1/end"
