# Redis Reference module

There is an easy to way to work with some sort of references in redis. You just
kepp the list of keys inside some other key. 

```
set list "item:1 item:2 item:3"
set item:1 "{id: 1}"
set item:2 "{id: 2}"
set item:3 "{id: 3}"
mget item:1 item:2 item:3
```

This works but it's possible to be faster. Let's see how this would work
in practice:

```python
recs = connection.get('list')
keys = recs.split(' ')
connection.mget(keys)
```

The key point is that it's impossible to achieve that without doing 2 gets
and one *userland* string split. Since 90% of what we do in redis is just 
get a list-of-items we thought we could improve this operation with
very simple native reference mechanism.

## How a reference work in practice?

Our recommendation engine selected the products `1`, `2` and `3` to some
user identified just by "user". 

This module will allow your application to query the list which the
content is `1,2,3` and natively getting `prefix:1` and then change 1 by 
`{id: 1, price:10}`, the key value, and so on.

```
set user 1,2,3
set store.com:iid:1 "{id: 1, price:10}"
set store.com:iid:2 "{id: 2, price:20}"
set store.com:iid:3 "{id: 3, price:30}"
reference.query store.com:iid: user
```

The last command returns:

```
{id: 1, price:10},{id: 2, price: 30},{id: 3, price:30}
```

With just one command we fetch the list of items.

```python
connection.execute_command('reference.query store.com:iid: user')
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

Source `t1.py`

```python
import redis

c = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

for i in range(10000):
    recs = c.get('user')
    keys = recs.split(' ')
    c.mget(keys)
```
Source `t2.py`

```python
import redis

c = redis.StrictRedis(host='127.0.0.1', port=6379, db=0)

for i in range(10000):
    all = c.execute_command('reference.query store.com user')
```

### Benchmark results

```
redis_reference ∞ λ  master* → time python t1.py
python t1.py  12.20s user 4.32s system 67% cpu 24.597 total
redis_reference ∞ λ  master* → time python t2.py
python t2.py  6.56s user 2.27s system 58% cpu 15.127 total
```

# Contact

`felipecruz@prognoos.com`
