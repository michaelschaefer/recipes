# recipes

A program to create, store, export and print recipes. (c) 2014 Michael Schaefer, [www.michael-schaefer.org/en/](http://www.michael-schaefer.org/en/)


## usage

The following paragraphs give a basic description on how to work with the program.


### working on recipes

A new recipe is either created from scratch by `File` ->  `New` or loaded via `File` -> `Open`. Once a recipe form exists you can use the different entries of the `Edit` menu to work on the recipe itself. They can be stored, exported and printed using the corresponding functions in the `File` menu.


### formatting hints

* By right-clicking the different parts of a recipe (i.e. ingredients and preparation steps) you get a context menu. This menu allows you to edit the corresponding block, remove it or move it up and down.
* It you need the ingredient names to have additional indentation, preceed them with a \t to add an additional tabulator.



## feature list

### recipe form

* headline
* ingredient list with serving count
* support for different ingredient sections
* ingredients with separated amount, unit and name
* preparation steps
* work on multiple recipes simultaneously

### management of recipes

* store as XML file
* export as PDF file
* printing support