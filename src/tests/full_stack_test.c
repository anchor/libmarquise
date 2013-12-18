#include <glib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../anchor_stats.h"
#include <zmq.h>

typedef struct {
        as_consumer *context;
        as_connection *connection;
} fixture;

void setup( fixture *f, gconstpointer td ){
        f->context = as_consumer_new("ipc:///tmp/as_full_stack_test", 0.1);
        g_assert( f->context );
        f->connection = as_connect(f->context);
        g_assert( f->connection );
}
void teardown( fixture *f, gconstpointer td ){
}

// Starting simple, we send one message and make sure we get it.
void one_message( fixture *f, gconstpointer td ){
        char *field_buf[] = {"foo"};
        char *value_buf[] = {"bar"};

        // Send a few messages
        g_assert( as_send_int( f->connection, field_buf, value_buf, 1, 10, 20 )
                  != -1 );

        // Now start up the server and expect them all.
        void *context = zmq_ctx_new();
        g_assert( context );
        void *bind_sock = zmq_socket( context, ZMQ_PULL );
        g_assert( bind_sock );
        g_assert( !zmq_bind( bind_sock, "ipc:///tmp/as_full_stack_test" ) );

        char *scratch = malloc(512);
        int recieved = zmq_recv( bind_sock, scratch, 512, 0 );
        g_assert_cmpint( recieved, ==, 27 );
        free( scratch );

        zmq_close( bind_sock );
        zmq_ctx_destroy( context );
        as_close( f->connection );
        as_consumer_shutdown( f->context );
}

// The aim here is to test a bunch of messages being sent without an upstream
// connection, so many that we are forced to defer to disk to save memory.
//
// Then we bring back the upstream connection and have all of the messages come
// through intact.
void defer_to_disk( fixture *f, gconstpointer td );


int main( int argc, char **argv ){
        g_test_init( &argc, &argv, NULL);
        g_test_add( "/full_stack/one_message"
                  , fixture
                  , NULL
                  , setup
                  , one_message
                  , teardown );
        return g_test_run();
}