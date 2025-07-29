#include "../headers/Subject.hpp"
#include "../headers/MoveEvent.hpp"
#include "../headers/CaptureEvent.hpp"
#include "../headers/GameStateEvent.hpp"
#include "../headers/TimeEvent.hpp"

// Explicit template instantiations
template class Subject<MoveEvent>;
template class Subject<CaptureEvent>;
template class Subject<GameStateEvent>;
template class Subject<TimeEvent>;
