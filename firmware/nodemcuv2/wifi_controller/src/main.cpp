#include "AutomaticFeeder.h"

Feeder::AutomaticFeeder feeder;

[[maybe_unused]] void setup() {
    feeder.Initialize();
}

void loop() {
    feeder.Update();
}
