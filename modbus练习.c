#include <stdio.h>
#include <modbus.h>
#include <pthread.h>
#include <unistd.h>

#define LED 0
#define BEEP 1

void *handler_info(void *arg)
{
    modbus_t *ctx = (modbus_t *)arg;
    uint16_t data[64] = {0};

    while (1)
    {
        modbus_read_registers(ctx, 0, 4, data);
        printf("光线:%d 加速度：x:%d,y:%d,z:%d\n", data[0], data[1], data[2], data[3]);
        sleep(3);
    }
}

void *handler_contrl(void *arg)
{
    modbus_t *ctx = (modbus_t *)arg;
    int dev, op;

    while (1)
    {
        scanf("%d %d", &dev, &op); // 1 0

        switch (dev)
        {
        case LED:
            modbus_write_bit(ctx, 0, op);
            break;
        case BEEP:
            modbus_write_bit(ctx, 1, op);
            break;
        }
    }
}

int main(int argc, const char *argv[])
{
    modbus_t *ctx;
    pthread_t tid1, tid2;

    ctx = modbus_new_rtu("/dev/ttyS1", 9600, 'N', 8, 1);
    if (ctx == NULL)
    {
        perror("new tcp err");
        return -1;
    }

    modbus_set_slave(ctx, 1);

    if (modbus_connect(ctx) < 0)
    {
        perror("connect err");
        modbus_free(ctx);
        return -1;
    }

    if (pthread_create(&tid1, NULL, handler_info, ctx) != 0)//pthread_create成功返回０
    {
        perror("create thread info err");
        return -1;
    }

    if (pthread_create(&tid2, NULL, handler_contrl, ctx) != 0)
    {
        perror("create thread contrl err");
        return -1;
    }

    pthread_join(tid1, NULL);
    pthread_join(tid2, NULL);

    modbus_close(ctx);
    modbus_free(ctx);

    return 0;
}
