import redis

c = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

for i in range(10000):
    all = c.execute_command('reference.query store.com user')

