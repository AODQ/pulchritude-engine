std::string mixinPluginSource = PULE_multilineString(
$$include <pulchritude-log/log.h>       \n
$$include <pulchritude-plugin/plugin.h> \n
                                        \n
extern "C" {                            \n
                                        \n
PulePluginType pulcPluginType() {       \n
  return PulePluginType_component;      \n
}                                       \n
                                        \n
void pulcComponentLoad() {              \n
  puleLog("Component loaded");          \n
}                                       \n
                                        \n
void pulcComponentUpdate() {            \n
}                                       \n
                                        \n
}
);
