# aklight
docker-compose build
docker-compose up

docker-compose down --remove-orphans
docker-compose build --no-cache
docker-compose up --remove-orphans

docker-compose down --volumes --remove-orphans
docker-compose build --no-cache
docker-compose up