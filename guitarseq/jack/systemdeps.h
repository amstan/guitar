#ifndef POST_PACKED_STRUCTURE

    #ifdef __GNUC__
        /* POST_PACKED_STRUCTURE needs to be a macro which
           expands into a compiler directive. The directive must
           tell the compiler to arrange the preceding structure
           declaration so that it is packed on byte-boundaries rather
           than use the natural alignment of the processor and/or
           compiler.
        */

        #define PRE_PACKED_STRUCTURE
        #define POST_PACKED_STRUCTURE __attribute__((__packed__))

    #else

        #ifdef _MSC_VER
            #define PRE_PACKED_STRUCTURE1 __pragma(pack(push,1))
            #define PRE_PACKED_STRUCTURE    PRE_PACKED_STRUCTURE1
            /* PRE_PACKED_STRUCTURE needs to be a macro which
            expands into a compiler directive. The directive must
            tell the compiler to arrange the following structure
            declaration so that it is packed on byte-boundaries rather
            than use the natural alignment of the processor and/or
            compiler.
            */
            #define POST_PACKED_STRUCTURE ;__pragma(pack(pop))
            /* and POST_PACKED_STRUCTURE needs to be a macro which
            restores the packing to its previous setting */
        #else
            #define PRE_PACKED_STRUCTURE
            #define POST_PACKED_STRUCTURE
        #endif /* _MSC_VER */

    #endif /* __GNUC__ */

#endif


typedef unsigned long int pthread_t;
typedef pthread_t jack_native_thread_t;
