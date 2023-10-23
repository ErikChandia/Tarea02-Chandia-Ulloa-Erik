#ifndef _CHECKARGS_H_
#define _CHECKARGS_H_

#include <getopt.h>  
#include <unistd.h>

extern char *optarg;
extern int optind, opterr, optopt;

class checkArgs {
private:
static struct option long_options[];
    const std::string opciones = "--threads N --file FILENAME [--help]";  // Agregar esta línea
    const std::string descripcion = "\n Modo de uso: ./histograma_mt --threads N --file FILENAME [--help]\n" 
                                    "\t--threads: cantidad de threads a utilizar. Si es 1, entonces ejecuta la versión secuencial.\n"
                                    "\t--file : archivo a procesar.\n"
                                    "\t--help : muestra este mensaje y termina.\n";


    typedef struct args_t {
        uint32_t numThreads;
        std::string filename;
    } args_t;

    args_t parametros;
    int argc;
    char **argv;

	
public:
	checkArgs(int _argc , char **_argv);
	~checkArgs();
	args_t getArgs();
	
private:
	void printUsage();
	
	
};

checkArgs::checkArgs(int _argc , char **_argv) {
    parametros.numThreads = 0;
    parametros.filename = "";

    argc = _argc;
    argv = _argv;

    // Si no hay argumentos o solo hay un argumento (--help)
    if (argc == 1 || (argc == 2 && std::string(argv[1]) == "--help")) {
        printUsage();
        exit(EXIT_SUCCESS);
    }
}

checkArgs::~checkArgs(){
	
}

checkArgs::args_t checkArgs::getArgs(){
    int opcion;
    int option_index = 0;
    
    static struct option long_options[] = {
        {"threads", required_argument, 0, 't'},
        {"file",    required_argument, 0, 'f'},
        {"help",    no_argument,       0, 'h'},
        {0,         0,                 0,  0 }
    };

    while ((opcion = getopt_long(argc, argv, "t:f:h", long_options, &option_index)) != -1){
        switch (opcion) {
            case 't':
                parametros.numThreads = std::stoi(optarg);
                break;
            case 'f':
                parametros.filename = optarg;
                break;
            case 'h':
            default:
                printUsage();
                exit(EXIT_SUCCESS);
        }
    }

    if (parametros.numThreads == 0 || parametros.filename.empty()){
        printUsage();
        exit(EXIT_FAILURE);
    }

    return parametros;
}


void checkArgs::printUsage(){
    std::cout << "Uso: " << argv[0] << " " << opciones << " " << descripcion << std::endl;
}




#endif
