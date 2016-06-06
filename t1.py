import redis

c = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

for i in range(10000):
    recs = c.get('user')
    objs = recs.split(',')
    all = ",".join([c.get('store.com:iid:' + k) for k in objs])
