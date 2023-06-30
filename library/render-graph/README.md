# Render Graph Node

Render graph is an acyclic graph of nodes that represent GPU operations. Each
node has a list of resources it requires and a list of dependencies on other
nodes that are required to execute on the GPU timeline prior to its own
execution.

This allows:
  1) simpler and automated resource synchronization management
  2) node command lists to be written to in parallel in any order


Each node has a list of command lists, one per-thread, that can be written to
across the lifetime of a frame and will be submitted to the GPU when they are
complete.

TODO move this elsewhere I guess
The lifetime of a frame:

  - puleGfxFrameStart (prepare/acquire next swapchain image)
  - pulePlatformPullEvents (poll user input, window/platform events, etc)
  - puleImguiNewFrame (create new imgui frame)


