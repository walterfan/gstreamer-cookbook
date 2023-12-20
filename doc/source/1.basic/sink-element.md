# Sink elements
Sink elements consume data and normally have no source pads.

Typical sink elements include:

* audio/video renderers
* network sinks
* filesinks

Sinks are harder to construct than other element types as they are treated specially by the GStreamer core.

## state changes

A sink always returns ASYNC from the state change to PAUSED, this includes a state change from READY→PAUSED and PLAYING→PAUSED. The reason for this is that this way we can detect when the first buffer or event arrives in the sink when the state change completes.

A sink should block on the first EOS event or buffer received in the READY→PAUSED state before committing the state to PAUSED.

FLUSHING events have to be handled out of sync with the buffer flow and take no part in the preroll procedure.

Events other than EOS do not complete the preroll stage.

