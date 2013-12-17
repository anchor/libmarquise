#include <glib.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "../structs.h"
#include "../defer.h"

typedef struct {
        deferral_file *df;
} fixture;

void setup( fixture *f, gconstpointer td ){
        f->df = as_deferral_file_new();
}
void teardown( fixture *f, gconstpointer td ){
        as_deferral_file_close(f->df);
}

void defer_then_read( fixture *f, gconstpointer td ){
        // two test bursts, we expect it to behave as a LIFO stack
        data_burst *first = malloc( sizeof( data_burst ) );
        first->data = malloc( 5 );
        strcpy( first->data, "first" );
        first->length = 6;

        data_burst *last = malloc( sizeof( data_burst ) );
        last->data = malloc( 5 );
        strcpy( last->data, "last" );
        last->length = 5;

        as_defer_to_file( f->df, first );
        as_defer_to_file( f->df, last );

        data_burst *first_retrieved = as_retrieve_from_file( f->df );
        g_assert_cmpuint( last->length, ==, first_retrieved->length);
        g_assert_cmpstr( last->data, ==, first_retrieved->data);


        data_burst *last_retrieved = as_retrieve_from_file( f->df );
        g_assert_cmpuint( first->length, ==, last_retrieved->length);
        g_assert_cmpstr( first->data, ==, last_retrieved->data);

        data_burst *nonexistant = as_retrieve_from_file( f->df );
        g_assert ( !nonexistant );

        free( first->data );
        free( last->data );
        free( first_retrieved->data );
        free( last_retrieved->data );
}

void defer_unlink_then_read( fixture *f, gconstpointer td ){
        data_burst *first = malloc( sizeof( data_burst ) );
        first->data = malloc( 5 );
        strcpy( first->data, "first" );
        first->length = 6;

        as_defer_to_file( f->df, first );
        unlink( f->df->path );
        data_burst *nonexistant = as_retrieve_from_file( f->df );
        g_assert ( !nonexistant );
}

void unlink_defer_then_read( fixture *f, gconstpointer td ){
        data_burst *first = malloc( sizeof( data_burst ) );
        first->data = malloc( 5 );
        strcpy( first->data, "first" );
        first->length = 6;

        unlink( f->df->path );
        as_defer_to_file( f->df, first );
        data_burst *first_retrieved = as_retrieve_from_file( f->df );
        g_assert( first_retrieved );
        g_assert_cmpuint( first->length, ==, first_retrieved->length);
        g_assert_cmpstr( first->data, ==, first_retrieved->data);
}


int main( int argc, char **argv ){
        g_test_init( &argc, &argv, NULL);
        g_test_add( "/set1/defer_then"
                  , fixture
                  , NULL
                  , setup
                  , defer_then_read
                  , teardown );
        g_test_add( "/set1/defer_unlink_then_read"
                  , fixture
                  , NULL
                  , setup
                  , defer_unlink_then_read
                  , teardown );
        g_test_add( "/set1/unlink_defer_then_read"
                  , fixture
                  , NULL
                  , setup
                  , unlink_defer_then_read
                  , teardown );
        return g_test_run();
}
