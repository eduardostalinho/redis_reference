import redis

c = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

for i in range(100000):
    recs = c.get('user')
    keys = recs.split(' ')
    c.mget(keys)
