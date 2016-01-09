#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/** Program specific. If class is available would change to OOP
 *  malloc on constructor and free on destructor. Ring buffer is
 *  used to buffer communications.
 *  @params zero: current 0 index.
 *  @params capacity: total buffer size.(should not be able to change
 *  after constructor)
 *  @params size:  current size of data written into buffer.  
 *  @params data: could hold capacity-1 characters. 
 *  data[BUFFER_SIZE] and last character stored in data are both 
 *  set to 0 to comply with C standard string functions.                         
 */
#define BUFFER_SIZE 10
typedef struct RingBuffer 
{
    int zero;
    int size;
    int capacity;
    char data[BUFFER_SIZE+10]; 
} RingBuffer;


/** Initialize the ring buffer. 
 */
RingBuffer* init_RingBuffer()
{
    RingBuffer* rb = (RingBuffer *)malloc(sizeof(RingBuffer));
    rb -> capacity = BUFFER_SIZE;
    rb -> zero = 0;
    rb -> size = 0;
    rb -> data[0] = 0;
    rb -> data[BUFFER_SIZE] = 0;
    return rb;
}


/** Write to buffer. 
 */
bool write_RingBuffer(RingBuffer *rb, const char *newdata) 
{
    int datalen = strlen(newdata);
    // if ring buffer too small, don't copy at all
    if(datalen > rb->capacity - rb->size - 1)
        return false;
    // if after copying doesn't surpass highest index
    if(rb->zero + rb->size + datalen < rb->capacity){
        strncpy(rb->data+rb->zero + rb->size,newdata,datalen);
        int last = rb->zero + rb->size + datalen;
        rb->data[last] = 0;
    // if existing data already surpassed highest index
    }else if(rb->zero + rb->size > rb->capacity){
        int firstavailable = rb->zero + rb->size % rb->capacity;
        strncpy(rb->data + firstavailable,newdata,datalen);
        int last = firstavailable + datalen;
        rb->data[last] = 0;
    // if newdata is split(including final 0 attached to index 0)
    }else{
        int l1 = rb->capacity - rb->zero - rb->size;
        int l2 = datalen - l1;
        strncpy(rb->data+rb->zero + rb->size,newdata, l1);
        strncpy(rb->data,newdata+l1, l2);
        rb->data[l2] = 0;
    }
    rb->size += datalen;
    return true;
}


/** Pop from buffer. incorrect
 */
bool pop_RingBuffer(RingBuffer *rb, char *result, int poplen)
{
    if(poplen <= rb->size){
        // split into two parts
        if(rb->zero + poplen > rb->capacity){
            int len1 = rb->capacity - rb->zero;
            int len2 = poplen - len1;
            strncpy(result,rb->data +rb->zero,len1);
            strncpy(result+len1,rb->data,len2);
            result[poplen] = 0;
        // in one piece
        }else{
            strncpy(result,rb->data + rb->zero,poplen);
            result[poplen] = 0;
        }
        const char * pointer = rb->data + rb->zero;
        rb->zero = (rb->zero+poplen)%rb->capacity;
        rb->size -= poplen;
        return true;
    }else{
        return false;
    }
}


int find_NormalString(const char*s,char c)
{
    char *found;
    found = strchr(s, c);
    if (found)
        return found - s;
    else
        return -1; 
}


/** Find char in Ring Buffer.
 *  @return: distance from "zero" or -1 for not found.
 */
int find_RingBuffer(RingBuffer *rb, char c)
{
    if(rb->zero+rb->size > rb->capacity){
        int findinl1 = find_NormalString(rb->data+rb->zero,c);
        if( findinl1 != -1){
            return findinl1;
        }else{
            return find_NormalString(rb->data,c) + rb->capacity-rb->zero;
        }
    }else{
        return find_NormalString(rb->data+rb->zero,c);
    }
}


/** Find char in normal string.
 *  @return: distance from "zero" or -1 for not found.
 */
void print_RingBuffer(RingBuffer *rb){
    printf("ZeroIndex:%d,Size:%d,Capacity:%d\n",rb->zero,
        rb->size,rb->capacity);
    if(rb->zero+rb->size > rb->capacity){
        printf("BUFFER:\"%s", rb->data+rb->zero);
        printf("%s\"\n",rb->data );
    }else{
        printf("BUFFER:\"%s\"\n", rb->data+rb->zero);
    }
}


/** Free rb at the end.
 */
void finish_RingBuffer(RingBuffer *rb){
    free(rb);
}

int main(int argc, char const *argv[])
{
    RingBuffer *rb = init_RingBuffer();
    printf("init\n");
    print_RingBuffer(rb);
    write_RingBuffer(rb,"123456789");
    printf("write123\n");
    print_RingBuffer(rb);
    char test[20];
    pop_RingBuffer(rb,test,9);
    printf("pop\n");
    print_RingBuffer(rb);
    printf("write123\n");
    write_RingBuffer(rb,"123456789");
    print_RingBuffer(rb);
    pop_RingBuffer(rb,test,9);
    printf("pop\n");
    print_RingBuffer(rb);
    write_RingBuffer(rb,"1234567890");
    printf("write too many, should fail\n");
    print_RingBuffer(rb);
    write_RingBuffer(rb,"12345678");
    printf("write123\n");
    print_RingBuffer(rb);
    pop_RingBuffer(rb,test,9);
    printf("pop too many, should fail\n");
    print_RingBuffer(rb);
    printf("Find '2' result:%d\n",find_RingBuffer(rb, '2'));
    printf("Find '5' result:%d\n",find_RingBuffer(rb, '5'));
    finish_RingBuffer(rb);
    return 0;
}
