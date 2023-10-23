# GstBus overview

The GstBus is an object responsible for delivering GstMessage packets in a first-in first-out way from the streaming threads (see GstTask) to the application.

> GstBus 是一个对象，从 streaming 线程以 first-in first-out 的方式负责交付 GstMessage 包到应用程序

Since the application typically only wants to deal with delivery of these messages from one thread, the GstBus will marshall the messages between different threads. This is important since the actual streaming of media is done in another thread than the application.

> 因为应用程序一般只想从一个线程来应对这些交付的消息， GstBus 会在不同的线程间安置消息
> 这挺重要，因为实际的流媒体是在另外一个线程中搞定的，而非应用程序线程。

The GstBus provides support for GSource based notifications. This makes it possible to handle the delivery in the glib GMainLoop.

The GSource callback function gst_bus_async_signal_func can be used to convert all bus messages into signal emissions.

A message is posted on the bus with the gst_bus_post method. With the gst_bus_peek and gst_bus_pop methods one can look at or retrieve a previously posted message.

The bus can be polled with the gst_bus_poll method. This methods blocks up to the specified timeout value until one of the specified messages types is posted on the bus. The application can then gst_bus_pop the messages from the bus to handle them. Alternatively the application can register an asynchronous bus function using gst_bus_add_watch_full or gst_bus_add_watch. This function will install a GSource in the default glib main loop and will deliver messages a short while after they have been posted. Note that the main loop should be running for the asynchronous callbacks.

It is also possible to get messages from the bus without any thread marshalling with the gst_bus_set_sync_handler method. This makes it possible to react to a message in the same thread that posted the message on the bus. This should only be used if the application is able to deal with messages from different threads.

Every GstPipeline has one bus.

Note that a GstPipeline will set its bus into flushing state when changing from READY to NULL state.