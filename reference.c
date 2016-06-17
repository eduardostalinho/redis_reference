#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "redismodule.h"

char * _getKey(RedisModuleCtx *ctx, char *c_key_str) {
    char *value;
    size_t len;
    RedisModuleKey *key;

    RedisModuleString *key_str =
        RedisModule_CreateString(ctx, c_key_str, strlen(c_key_str));

    key = RedisModule_OpenKey(ctx, key_str, REDISMODULE_READ);
    value = RedisModule_StringDMA(key, &len, REDISMODULE_READ);

    RedisModule_CloseKey(key);
    return value;
}


char** split_in(char *str, char sep, int *count) {
    int i = 0;
    char **values;
    char nsep[2] = {sep, '\0'};

    for (i = 0; i < strlen(str); i++) {
        if (str[i] == sep) {
            (*count)++;
        }
    }

    (*count)++;
    i = 0;

    values = (char**) malloc(sizeof(char*) * (*count));

    char *current = strtok(str, nsep);

    while (current != NULL) {
        values[i] = current;
        current = strtok(NULL, nsep);
        i++;
    }

    return values;
}

char* rconcat_results(RedisModuleCtx *ctx, char **results, int len, char sep) {
    int total_size = 0;
    int i = 0, j = 0, p = 0;
    char *final;

    for (i = 0; i < len; i++) {
        total_size += strlen(_getKey(ctx, results[i]));
   }

    total_size += (len - 1);

    final = (char*) malloc(sizeof(char) * total_size + 1);

    for (p = 0, i = 0; i < len; i++) {
        char *current = _getKey(ctx, results[i]);
        for (j = 0; j < strlen(current); j++, p++) {
            final[p] = current[j];
        }
        final[p++] = sep;
    }

    final[total_size] = '\0';

    return final;
}

int RefQuery(RedisModuleCtx *ctx, RedisModuleString **argv, int argc) {
    char *prefix,
         *list_value,
         *query_result,
         *original_key,
         *original_list_value;

    char **keys,
         **list_values;

    size_t j,
           len,
           cid_len,
           original_key_len;

    int i = 0,
        list_len = 0;

    RedisModuleKey *key;
    RedisModuleString *redis_str;

    if (argc < 3) {
        return RedisModule_WrongArity(ctx);
    }

    prefix = (char*) RedisModule_StringPtrLen(argv[1], &cid_len);
    original_key = (char*) RedisModule_StringPtrLen(argv[2], &original_key_len);

    key = RedisModule_OpenKey(ctx, argv[2], REDISMODULE_READ);
    len = RedisModule_ValueLength(key);

    original_list_value = RedisModule_StringDMA(key, &len, REDISMODULE_READ);
    list_value = malloc(sizeof(char) * (len + 1));
    strcpy(list_value, original_list_value);

    list_values = split_in(list_value, ',', &list_len);
    keys = (char**) malloc(sizeof(char*) * list_len);

    for (i = 0; i < list_len; i++) {
        keys[i] = (char*) malloc(
            sizeof(char) * (1 + strlen(prefix) + strlen(list_values[i])));
        snprintf(keys[i], 1 + strlen(prefix) + strlen(list_values[i]),
                 "%s%s", prefix, list_values[i]);
    }

    query_result = rconcat_results(ctx, keys, list_len, ':');

    for (i = 0; i < list_len; i++) {
        free(keys[i]);
    }

    RedisModule_CloseKey(key);

    redis_str = RedisModule_CreateString(ctx, query_result,
                                         strlen(query_result));

    free(keys);
    free(list_values);
    free(list_value);
    free(query_result);

    return RedisModule_ReplyWithString(ctx, redis_str);
}

int RedisModule_OnLoad(RedisModuleCtx *ctx) {
    if (RedisModule_Init(ctx, "reference", 1,
                       REDISMODULE_APIVER_1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
    if (RedisModule_CreateCommand(ctx, "reference.query", RefQuery,
                                "readonly", 1,1,1) == REDISMODULE_ERR) {
        return REDISMODULE_ERR;
    }
}
