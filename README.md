# Redis Reference module

Before understand why we need references in redis check our use-case.

## Our use-case

We store thousands of lists of items. So we basically have two types of objects:

* Items
* Lists of items

An item is inside one or more lists. A popular item will be inside many
lists. This model has 2 big problems:

* Space
* Update complexity

### Space problem

Suppose you have a very popular item inside thousands of lists. Without references
you'll have to keep the whole item information inside all of those lists but
the content of the item is just one so in practice this model has many copies
of the same item thus wasting memory space.

### Update problem

If you keep coopies of the item content inside many lists once this item has
some attribute updates, you'll need to update all copies. This is not efficient
specially for popular itens. Another problem is that you'll have to keep track
of what lists contains a particular item.

## How a reference work in practice?

Our recommendation engine selected the products 1, 2 and 3 to some user identified
just by "user9". This module will allow your application to query the list which the
content is `1,2,3` and natively change 1 by `{id: 1, price:10}` and so on.

```
set user9 1,2,3
set store.com:iid:1 "{id: 1, price:10}"
set store.com:iid:2 "{id: 2, price:20}"
set store.com:iid:3 "{id: 3, price:30}"

reference.query store.com:iid user9
```

In order to do that you must specify a key prefix `sotore.com:iid` and
the original list key. They query above will return:

```
{id: 1, price:10},{id: 2, price: 30},{id: 3, price:30}
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
