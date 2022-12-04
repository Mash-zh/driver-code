#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#define LEDOFF 1
#define LEDON 0

int main(int argc, char *argv[])
{
    int fd, ret = 0;
    unsigned char databuf[1];

    if(argc != 3){
        printf("usage: %s device_node status(0 or 1)!\n", argv[0]);
        ret = -1;
        return ret;
    }

    char *pDevice_node = argv[1];
    databuf[0] = atoi(argv[2]);

    //open file
    fd = open(pDevice_node, O_RDWR);
    if(fd < 0){
        printf("device node %s open failed !\n", pDevice_node);
        return fd;
    }
    printf("open success\n");
    ret = write(fd, databuf, sizeof(databuf));
    if(ret < 0){
        printf("LED control failed !\n", pDevice_node);
        close(fd);
        return ret;
    }
    printf("write success\n");
    ret = close(fd);
    if(ret < 0){
        printf("device node %s close failed !\n", pDevice_node);
        return ret;
    }
    return ret;
}
