#include <global.hh>
#include <checkArgs.hpp>

std::map<std::string, int> wordHistogram;
std::vector<std::string> splitText(const std::string &text);
std::mutex myMutex;

std::string removePunctuation(const std::string& word) {
    std::string result;
    for (char c : word) {
        if (std::isalpha(c)) {
            result += c;
        }
    }
    return(result);
}

//donde se hace la pega
void processText(const std::vector<std::string>& lines, int threadId) {
    for (const auto& line : lines) {
        auto words = splitText(line);
        for (const auto& word : words) {
            {
                std::lock_guard<std::mutex> lock(myMutex); // Bloquear el mutex solo para actualizar el contador
                wordHistogram[word]++; //Critico se pueden sobre escribir un hilo sobre totro
            }
        }
    }
}


// Función para dividir un texto en palabras
std::vector<std::string> splitText(const std::string &text) {
    std::vector<std::string> words;
    std::string word;
    std::istringstream iss(text);
    while (iss >> word) {
		
        //std::string lowercaseWord = word;
        for (char &c : word) {
            c = std::tolower(c);
        }
		
		word = removePunctuation(word);
        words.push_back(word);
    }
    return words;
}

int main(int argc, char* argv[]) {

    checkArgs* argumentos = new checkArgs(argc, argv); //Se va al archivo include/checkArgs.hpp y revisa que tenga todo las variables
    auto args = argumentos->getArgs(); // recuperas los argumentos
    delete argumentos;

    std::cout << "Número de threads: " << args.numThreads << std::endl;
    std::cout << "Archivo a procesar: " << args.filename << std::endl;
	std::vector<std::string> textInMemory;
	
    std::string fileName = args.filename;  // Usar el nombre de archivo provisto como argumento

    std::ifstream file(fileName); 
	
    if (!file) {
        std::cerr << "No se pudo abrir el archivo." << std::endl;
        return(EXIT_FAILURE);
    }
	
    std::string line;
    while (std::getline(file, line)) {
		textInMemory.push_back(line); // Copia todo el archivo en memoria linea por linea 
	}
	file.close();
	

    int numThreads = args.numThreads;
    
if (numThreads == 1) {
    // Modo secuencial
    processText(textInMemory, 0);  // 0 como ID para el hilo principal
} else {
    // Modo paralelo

    //Reparticion de pega sin hacer la pega
    std::vector<std::thread> threads;
    int linesPerThread = textInMemory.size() / numThreads;
    for (int i = 0; i < numThreads; ++i) {
        auto start = std::begin(textInMemory) + i * linesPerThread;
        auto end = (i == numThreads - 1) ? std::end(textInMemory) : start + linesPerThread;
        threads.push_back(std::thread(processText, std::vector<std::string>(start, end), i));
    }



    //ESperoa que la pega termine 
    for (auto& t : threads) {
        t.join();
    }



}




    // Mostrar el histograma de palabras
    for (const auto &entry : wordHistogram) {
        std::cout << entry.first << ": " << entry.second << std::endl;
    }


    return(EXIT_SUCCESS);
}


