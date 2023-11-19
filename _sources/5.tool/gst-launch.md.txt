# gst-launch


## Description
gst-launch-1.0 is a tool that builds and runs basic GStreamer pipelines.

In its simplest form, a PIPELINE-DESCRIPTION is a list of elements separated by exclamation marks (!). Properties may be appended to elements in the form property=value.

## Usage

```
gst-launch-1.0 [OPTIONS] PIPELINE-DESCRIPTION
```

例如

```
gst-launch-1.0 -v -m avfvideosrc device-index=0 ! autovideosink 
```

## Options


gst-launch-1.0 accepts the following options:

```
--help

Print help synopsis and available FLAGS

-v, --verbose

Output status information and property notifications

-q, --quiet

Do not print any progress information

-m, --messages

Output messages posted on the pipeline's bus

-t, --tags

Output tags (also known as metadata)

-o FILE, --output=FILE

Save XML representation of pipeline to FILE and exit

-f, --no_fault

Do not install a fault handler

-T, --trace

Print memory allocation traces. The feature must be enabled at compile time to work.

```

## Pipeline Description

A pipeline consists of elements and links. Elements can be put into bins of different sorts. Elements, links, and bins can be specified in a pipeline description in any order.

* Elements
```
ELEMENTTYPE [PROPERTY1 ...]
```

Creates an element of type ELEMENTTYPE and sets its PROPERTIES.

* Element Properties
```
PROPERTY=VALUE ...
```

Sets the property to the specified value. You can use gst-inspect-1.0 to find out about properties and allowed values of different elements. Enumeration properties can be set by name, nick or value.

* Bins
```
[BINTYPE.] ([PROPERTY1 ...] PIPELINE-DESCRIPTION)
```

Specifies that a bin of type BINTYPE is created and the given properties are set. Every element between the braces is put into the bin. Please note the dot that has to be used after the BINTYPE. You will almost never need this functionality, it is only really useful for applications using the gst_parse_launch() API with bin as bintype. That way it is possible to build partial pipelines instead of a full-fledged top-level pipeline.

* Links
```
[[SRCELEMENT\].[PAD1,...]] ! [[SINKELEMENT].[PAD1,...]]
```

Links the element with name SRCELEMENT to the element with name SINKELEMENT. Names can be set on elements using the name property. If the name is omitted, the element that was specified directly in front of or after the link is used. 

This works across bins. If a padname is given, the link is done using that pad. If no pad names are given all possibilities are tried and a compatible pad is used. If multiple padnames are given, both sides must have the same number of pads specified and multiple links are done in the given order. The simplest link is a simple exclamation mark. This links the element to the left of it with the element at its right.

The following links the element with name SRCELEMENT to the element with name SINKELEMENT, using the caps specified in CAPS as a filter:

```
[[SRCELEMENT].[PAD1,...]] ! CAPS ! [[SINKELEMENT].[PAD1,...]]
```
* Caps
```
MIMETYPE [, PROPERTY[, PROPERTY ...]]] [; CAPS[; CAPS ...]]
```

Creates a capability with the given mimetype and optionally with given properties. The mimetype can be escaped using " or '. If you want to chain caps, you can add more caps in the same format afterwards.

* Caps Properties
```
NAME=[(TYPE)] VALUE in lists and ranges: [(TYPE)] VALUE
```

Sets the requested property in capabilities. The name is an alphanumeric value and the type can have the following case-insensitive values:

```
i or int for integer values or ranges;
f or float for float values or ranges;
4 or fourcc for FOURCC values;
b, bool, or boolean for boolean values;
s, str, or string for strings;
fraction for fractions (framerate, pixel-aspect-ratio);
l or list for lists.
```

If no type was given, the following order is tried: integer, float, boolean, string. Integer values must be parsable by strtol(), floats by strtod(). FOURCC values may either be integers or strings. Boolean values are (case insensitive) yes, no, true or false and may like strings be escaped with " or '. Ranges are in this format: [ VALUE, VALUE]; lists use this format: (VALUE [, VALUE ...]).