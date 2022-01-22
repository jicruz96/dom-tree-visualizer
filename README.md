# HTML-to-DOM Tree Graphic, from absolute scratch

**So I wanted to learn HTML, CSS, Javascript so I can make web pages.**

And then I was like "yo how does any of this work. Like, how does this stuff get interpreted, rendered, etc..."

And like, who's in charge of making the standards that define these languages and how they work on the web? (spoiler alert: Google, Apple, Mozilla, basically are solely responsible for defining the standards that determine 99.9% of web traffic)

And so I was like, "eff it", I'm building a web browser.

And then I read [this stackoverflow post](https://stackoverflow.com/questions/598841/how-to-get-started-building-a-web-browser) that was basically like "ha! there's no way you can build a web browser. it's like impossibly hard"

and now I'm like, yeah, probably.

But I'm still like, damn, I wanna do something with C again. Like it's soooooooo impractical for building scalable things fast but I feel so at home in the language and kind of miss it and damn I just hate HTML and CSS.

So, say yes to irrationally-derived joy out of working with obsolete tools to make obsolete things (after all, no one ever judges people who knit clothing. it's clearly just a hobby) and say no to the haters!

## So, I'm going to start learning HTML and CSS by building a program that can convert a webpage into a webpage that displays an interactive graph of its DOM Tree. The whole thing will be built in C and will use as few libraries as possible.

each node in the DOM Tree should display the element type, its children, its style attributes, and all its metadata.

if you're here, then you're welcome to contribute.

I will be livestreaming my work sessions, and you're welcome to stop by there too. If you're a Holberton student and have a C-related question, come through and we can treat the thing like office hours.

## To-Dos

Okay, so far all I have is a rough as hell, very hastily written first go at an HTML parser. Next I have to:

* Refactor existing code into something manageable & neat
  * Top priority: `make_dom_node` - a recursive mess of a function (that works!) but just makes no gosh darn sense.
* Make a Makefile
* Documentation, documentation, documentation, documentation
* Research how to incorporate [graphviz](https://graphviz.org/)
* Research CI/CD tools to enforce code formatting and documentation
