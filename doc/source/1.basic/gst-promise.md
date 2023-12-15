# GstPromise

The GstPromise object implements the container for values that may be available later. i.e. a Future or a Promise in https://en.wikipedia.org/wiki/Futures_and_promises. 

As with all Future/Promise-like functionality, there is the concept of the producer of the value and the consumer of the value.

A GstPromise is created with `gst_promise_new` by the consumer and passed to the producer to avoid thread safety issues with the change callback. 

A GstPromise can be replied to with a value (or an error) by the producer with `gst_promise_reply`. The exact value returned is defined by the API contract of the producer and NULL may be a valid reply. 

`gst_promise_interrupt` is for the consumer to indicate to the producer that the value is not needed anymore and producing that value can stop. 

The `GST_PROMISE_RESULT_EXPIRED` state set by a call to `gst_promise_expire` indicates to the consumer that a value will never be produced and is intended to be called by a third party that implements some notion of message handling such as GstBus. 

A callback can also be installed at GstPromise creation for result changes with `gst_promise_new_with_change_func`. The change callback can be used to chain GstPromise's together as in the following example.


```
const GstStructure *reply;
GstPromise *p;
if (gst_promise_wait (promise) != GST_PROMISE_RESULT_REPLIED)
    return; // interrupted or expired value
reply = gst_promise_get_reply (promise);
if (error in reply)
    return; // propagate error
p = gst_promise_new_with_change_func (another_promise_change_func, user_data, notify);
//pass p to promise-using API
```


Each GstPromise starts out with a `GstPromiseResult` of `GST_PROMISE_RESULT_PENDING` and only ever transitions once into one of the other GstPromiseResult's.

In order to support multi-threaded code, `gst_promise_reply`, `gst_promise_interrupt` and `gst_promise_expire` may all be from different threads with some restrictions and the final result of the promise is whichever call is made first. There are two restrictions on ordering:

That `gst_promise_reply` and `gst_promise_interrupt` cannot be called after `gst_promise_expire`
That `gst_promise_reply` and `gst_promise_interrupt` cannot be called twice.

The change function set with `gst_promise_new_with_change_func` is called directly from either the gst_promise_reply, gst_promise_interrupt or gst_promise_expire and can be called from an arbitrary thread. GstPromise using APIs can restrict this to a single thread or a subset of threads but that is entirely up to the API that uses GstPromise.