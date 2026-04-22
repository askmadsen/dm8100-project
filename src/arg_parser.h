#define ARG_INIT() int i = 1;
#define STR_ARG(var) if (argc >= i) { *var = argv[i++]; } else { return -1; }
#define INT_ARG(var) if (argc >= i) { *var = atoi(argv[i++]); } else { return -1; }
#define OPT_START() while (i < argc) {
#define OPT_END() return -1; }
#define OPT_STR_ARG(arg, var) if (!strcmp(argv[i], arg)) { i++; if (i < argc) { *var = argv[i]; } i++; } else
#define OPT_INT_ARG(arg, var) if (!strcmp(argv[i], arg)) { i++; if (i < argc) { *var = atoi(argv[i]); } i++; } else