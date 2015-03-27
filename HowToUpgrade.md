#summary How to upgrade gdipp from version to version
#labels Phase-Deploy

This text describes the key points of how to upgrade gdipp from version to version. It is organized incrementally against each released version. If the gdipp you plan to upgrade to is several versions newer than your current one, it is recommended to follow the guideline strictly and not to skip those intermediate.

So far, the general upgrade steps follow the pattern:

**_old\_version_ -> _new\_version_**
  1. Uninstall the _old\_version_.
  1. Install the _new\_version_. Installer will backup the _old\_version_ setting file if exists.
  1. Overwrite the _new\_version_ setting file with the backed up _old\_version_ one.
  1. Modify the overwritten setting file as described below.
  1. Alternatively, you can use the _new\_version_ default setting file and customize it.


**0.9.0 -> 0.9.1**
> No additional step is required.


**0.8.2 -> 0.9.0**
  1. You can use `<process>` and `<font>` to substitute selector nodes which selects any object, like `<process name=".*">` and `<font name=".+">`.
  1. Remove the `<max_height>` setting node from all `<font>` selector nodes. Instead, put the **max\_height** optional attributes to the `<font>` selector nodes with the same setting value.
  1. Use the following fail-safe `<font>` selector node as the last selector node:
```
<font>
  <renderer>0</renderer>
</font>
```
  1. Add `<thread>` setting node in the `<demo>` node.


**0.8.1 -> 0.8.2**
> Remove the `<service>` node.


**Any older version -> 0.8.1**
> Customize the 0.8.1 default setting file.