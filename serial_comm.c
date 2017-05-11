#include "serial_comm.h"

#define READ_USLEEP 100

int fd;
distance struct_distances;

char serialport_read_char();

int serialport_init(){
    struct termios toptions;
    int n;
    char c;

    fd = open(PORT_NAME, O_RDWR | O_NOCTTY | O_NDELAY);
    
    if (fd == -1){
        perror("init_serialport: Unable to open port ");
        return -1;
    }
    
    if (tcgetattr(fd, &toptions) < 0){
        perror("init_serialport: Couldn't get term attributes");
        return -1;
    }
    
    speed_t brate = BAUD;

    cfsetispeed(&toptions, brate);
    cfsetospeed(&toptions, brate);

    // 8N1
    toptions.c_cflag &= ~PARENB;
    toptions.c_cflag &= ~CSTOPB;
    toptions.c_cflag &= ~CSIZE;
    toptions.c_cflag |= CS8;
    // no flow control
    toptions.c_cflag &= ~CRTSCTS;

    toptions.c_cflag |= CREAD | CLOCAL;  // turn on READ & ignore ctrl lines
    toptions.c_iflag &= ~(IXON | IXOFF | IXANY); // turn off s/w flow ctrl

    toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG); // make raw
    toptions.c_oflag &= ~OPOST; // make raw

    // see: http://unixwiz.net/techtips/termios-vmin-vtime.html
    toptions.c_cc[VMIN]  = 0;
    toptions.c_cc[VTIME] = 0;
    
    if( tcsetattr(fd, TCSANOW, &toptions) < 0){
        perror("init_serialport: Couldn't set term attributes");
        return -1;
    }

    //clear buffer
    do{
        n = read(fd, &c, 1);
    	if(n == 0) {
            usleep(READ_USLEEP);
            break;
        }else if(n == -1){
        	break;
        }
    }while(1);
    
    tcflush(fd, TCIFLUSH);
    printf("INIT finished\n");
    
    return fd;
}

int serialport_read(){
    char c, buffer[10];
    int n, i;
    
    do{ 
        n = read(fd, &c, 1);
        if(n == -1){
            printf("ERROR READING FROM SERIALPORT\n");
            return -1;
        }
        if(n == 0) {
            usleep(READ_USLEEP);
            continue;
        }else if(c == 'S'){

            //Read distance front sensor
            for(i = 0; 1; i++){
                buffer[i] = serialport_read_char();
                if(buffer[i] == '\n'){
                    buffer[i] = '\0';
                    struct_distances.front = (float)atof(buffer);
                    break;
                }
            }
            //Read distance Back sensor
            /*for(i = 0; 1; i++){
                buffer[i] = serialport_read_char();
                if(buffer[i] == '\n'){
                    buffer[i] = '\0';
                    struct_distances.back = (float)atof(buffer);
                    break;
                }
            }*/
            //Read distane left sensor
            for(i = 0; 1; i++){
                buffer[i] = serialport_read_char();
                if(buffer[i] == '\n'){
                    buffer[i] = '\0';
                    struct_distances.left = (float)atof(buffer);
                    break;
                }
            }
            //Read distance right sensor
            for(i = 0; 1; i++){
                buffer[i] = serialport_read_char();
                if(buffer[i] == '\n'){
                    buffer[i] = '\0';
                    struct_distances.right = (float)atof(buffer);
                    break;
                }
            }

            printf("front-> %f\nback-> %f\nleft-> %f\nright-> %f\n\n\n", struct_distances.front, struct_distances.back, struct_distances.left, struct_distances.right);
            break;
        }
    }while(1);
    
    return 0;
}

char serialport_read_char(){
    char c;
    int n = 0;
    
    do{ 
        n = read(fd, &c, 1);
        if(n == -1){
            printf("ERROR READING FROM SERIALPORT\n");
            exit(0);
        }
        if(n == 0) {
            usleep(READ_USLEEP);
            continue;
        }
        else{
            return c;
        }
    }while(n == 0);

    return 'Z';
}