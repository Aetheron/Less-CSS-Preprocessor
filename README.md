
LESS CSS Preprocessor
=====================

This module allows for automatic compilation of LESS style sheets.

Requirements
------------

LESS requires at least one of two possible engines available:

[oyejorge/less.php]: http://lessphp.gpeasy.com/
[less.js]: http://lesscss.org/usage/#command-line-usage

[Command Line Requirements]: #command

 -  [oyejorge/less.php] *archived*
    
    This is a pure PHP implementation, which is good for shared hosting, or if you don't feel comfortable
    installing or configuring software on your server.
    
    It lacks the ability to execute javascript embedded in .less files, so some LESS libraries might not work.
    
    Requirements:
    
    1. [Libraries](https://drupal.org/project/libraries)
    2. [oyejorge/less.php] installed such that `Less.php` is located at `sites/all/libraries/less.php/Less.php`
    
    This project has been archived by the author and is extremely out of date. It does not include support for
    LESS features released since v2, and will break when they are encountered. Use of this engine is highly
    discouraged.

 -  [less.js]
    
    You can read about how to install here: [less.js]
    
    Please read [Command Line Requirements] to ensure that *less.js* is installed properly.

Once installed, you must select your engine on 'admin/config/development/less'.

Less engines written in PHP do not support embedded JavaScript evaluation.

Optional
--------

### Autoprefixer

[postcss/autoprefixer]: https://github.com/postcss/autoprefixer

[postcss/autoprefixer]
> Write your CSS rules without vendor prefixes (in fact, forget about them entirely)
> 
> Autoprefixer will use the data based on current browser popularity and property support to apply prefixes for you.

Please read [Command Line Requirements] to ensure that *autoprefixer* is installed properly.

<a name="command"></a>
Command Line Requirements
-------------------------

Both [less.js] and [postcss/autoprefixer] require that PHP can call these programs directly from the command line.

It is up to the installer (you) to ensure that the `lessc` and/or `autoprefixer` are able to be called by PHP without a full path to the binary.

Thus is it required that you configure your PATH for the user that your PHP installation runs under to ensure that these programs can be run by name without the full path.


LESS Development:

Syntax: http://lesscss.org/features/



File placement:  
If your source file was `sites/all/modules/test/test.css.less`  
Then your compiled file will be `sites/[yoursite]/files/less/[random.string]/sites/all/modules/test/test.css`  

Usage
-----

Include less files in your `module.library.yml` just as you would with regular css files

[module.library.yml]:

    source:
      version: 1.x
      css:
        theme:
          less/source.css.less: {}


Compatibility
-------------

Should work with most themes and caching mechanisms.


### CSS Aggregation

Fully compatible with "Optimize CSS files" setting on "Admin->Site configuration->Performance" (admin/settings/performance).


### RTL Support

RTL support will work as long as your file names end with ".css.less".

Assuming your file is named "somename.css.less", Drupal automatically looks for a file name "somename-rtl.css.less"

Variables
---------

Variable defaults can be defined in `.theme` and `.module` files using `hook_less_variables_alter`

.theme file:

    function hook_theme_less_variables_alter( array &$less_variables, $system_name )
    {
      	$less_variables['@defaults'] = '~"theme/less/defaults.less"';
	      $less_variables['@styles'] = '~"theme/less"';
	      $less_variables['@theme_images'] = '~"/img"';
    }
