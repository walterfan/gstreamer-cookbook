# gobject overview


## g_signal_emit_by_name
```
void
g_signal_emit_by_name (
  GObject* instance,
  const gchar* detailed_signal,
  ...
)
```

### Description

Emits a signal. Signal emission is done synchronously. The method will only return control after all handlers are called or signal emission was stopped.

Note that g_signal_emit_by_name() resets the return value to the default if no handlers are connected, in contrast to g_signal_emitv().

This function is not directly available to language bindings.


