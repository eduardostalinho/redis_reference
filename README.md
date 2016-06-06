# Redis Reference module

**WARNING** Don't use this on production!

This module allow users to query objects by reference. 

```
set user 1,2,3
set store.com:iid:1 "{id: 1, price:10}"
set store.com:iid:2 "{id: 2, price:20}"
set store.com:iid:3 "{id: 3, price:30}"

reference.query store.com user
```

## Building

`make`

## Running

`redis-server --loadmodule ./reference.so`

## Functional Testing

The command above should work

```
redis-cli < check.txt
```

## Code examples

Source t1.py

```python
import redis

c = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

for i in range(10000):
    recs = c.get('user')
    objs = recs.split(',')
    all = ",".join([c.get('store.com:iid:' + k) for k in objs])
```
Source t2.py

```python
import redis

c = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

for i in range(10000):
    all = c.execute_command('reference.query store.com user')
```

### Benchmark results

```
redis_reference ∞ λ  master* → time python b1.py
python b1.py  6.60s user 2.38s system 66% cpu 13.475 total
redis_reference ∞ λ  master* → time python b2.py
python b2.py  0.70s user 0.27s system 60% cpu 1.600 total
```

# Contact

`felipecruz@prognoos.com`
