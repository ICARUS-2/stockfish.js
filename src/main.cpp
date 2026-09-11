#include <iostream>
#include <memory>
#include <utility>

#include "attacks.h"
#include "misc.h"
#include "position.h"
#include "tune.h"
#include "uci.h"

using namespace Stockfish;

#ifdef __EMSCRIPTEN__
UCIEngine* uciP; // Create a global pointer to the UCI object
#ifndef __EMSCRIPTEN_SINGLE_THREADED__
bool ready = false;
#endif
#endif

#ifdef UNIVERSAL_BINARY
namespace Stockfish {

    int main(int argc, char* argv[]);  // silence 'no previous declaration'

    __attribute__((used)) // keep main alive
    #endif

    int main(int argc, char* argv[]) {
        std::cout << engine_info() << std::endl;

        Attacks::init();
        Position::init();

        #ifndef __EMSCRIPTEN__
        auto cli = CommandLine(argc, argv);
        auto uci = std::make_unique<UCIEngine>(std::move(cli));

        Tune::init(uci->engine_options());

        uci->loop();
        #else
        // Emscripten cannot use the blocking uci->loop()
        auto cli = CommandLine(argc, argv);
        uciP = new UCIEngine(std::move(cli));
        Tune::init(uciP->engine_options());

        #ifndef __EMSCRIPTEN_SINGLE_THREADED__
        ready = true;
        #endif
        #endif

        return 0;
    }

    #ifdef __EMSCRIPTEN__
    extern "C" void command(const char *cmd) {
        uciP->process_command(cmd);
    }

    extern "C" bool isReady() {
        return ready;
    }
    #endif

    #ifdef UNIVERSAL_BINARY
}  // namespace Stockfish

#ifdef UNIVERSAL_NEEDS_MAIN_SHIM
int main(int argc, char* argv[]) { return Stockfish::main(argc, argv); }
#endif
#endif
