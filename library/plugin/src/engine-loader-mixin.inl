  // allocator
  layer->allocateDefault = (
    reinterpret_cast<decltype(layer->allocateDefault)>(
      ::loadSymbol("puleAllocateDefault", layerName)
    )
  );
  layer->allocate = (
    reinterpret_cast<decltype(layer->allocate)>(
      ::loadSymbol("puleAllocate", layerName)
    )
  );
  layer->reallocate = (
    reinterpret_cast<decltype(layer->reallocate)>(
      ::loadSymbol("puleReallocate", layerName)
    )
  );
  layer->deallocate = (
    reinterpret_cast<decltype(layer->deallocate)>(
      ::loadSymbol("puleDeallocate", layerName)
    )
  );
  // array
  layer->array = (
    reinterpret_cast<decltype(layer->array)>(
      ::loadSymbol("puleArray", layerName)
    )
  );
  layer->arrayDestroy = (
    reinterpret_cast<decltype(layer->arrayDestroy)>(
      ::loadSymbol("puleArrayDestroy", layerName)
    )
  );
  layer->arrayAppend = (
    reinterpret_cast<decltype(layer->arrayAppend)>(
      ::loadSymbol("puleArrayAppend", layerName)
    )
  );
  layer->arrayElementAt = (
    reinterpret_cast<decltype(layer->arrayElementAt)>(
      ::loadSymbol("puleArrayElementAt", layerName)
    )
  );
  // asset-asset-script-task-graph
  layer->assetScriptTaskGraphFromPds = (
    reinterpret_cast<decltype(layer->assetScriptTaskGraphFromPds)>(
      ::loadSymbol("puleAssetScriptTaskGraphFromPds", layerName)
    )
  );
  // asset-image
  layer->assetImageExtensionSupported = (
    reinterpret_cast<decltype(layer->assetImageExtensionSupported)>(
      ::loadSymbol("puleAssetImageExtensionSupported", layerName)
    )
  );
  layer->assetImageLoadFromStream = (
    reinterpret_cast<decltype(layer->assetImageLoadFromStream)>(
      ::loadSymbol("puleAssetImageLoadFromStream", layerName)
    )
  );
  layer->assetImageDestroy = (
    reinterpret_cast<decltype(layer->assetImageDestroy)>(
      ::loadSymbol("puleAssetImageDestroy", layerName)
    )
  );
  layer->assetImageDecodedData = (
    reinterpret_cast<decltype(layer->assetImageDecodedData)>(
      ::loadSymbol("puleAssetImageDecodedData", layerName)
    )
  );
  layer->assetImageDecodedDataLength = (
    reinterpret_cast<decltype(layer->assetImageDecodedDataLength)>(
      ::loadSymbol("puleAssetImageDecodedDataLength", layerName)
    )
  );
  layer->assetImageWidth = (
    reinterpret_cast<decltype(layer->assetImageWidth)>(
      ::loadSymbol("puleAssetImageWidth", layerName)
    )
  );
  layer->assetImageHeight = (
    reinterpret_cast<decltype(layer->assetImageHeight)>(
      ::loadSymbol("puleAssetImageHeight", layerName)
    )
  );
  // asset-tiled
  layer->assetTiledMapInfo = (
    reinterpret_cast<decltype(layer->assetTiledMapInfo)>(
      ::loadSymbol("puleAssetTiledMapInfo", layerName)
    )
  );
  layer->assetTiledMapLoadFromStream = (
    reinterpret_cast<decltype(layer->assetTiledMapLoadFromStream)>(
      ::loadSymbol("puleAssetTiledMapLoadFromStream", layerName)
    )
  );
  layer->assetTiledMapLoadFromFile = (
    reinterpret_cast<decltype(layer->assetTiledMapLoadFromFile)>(
      ::loadSymbol("puleAssetTiledMapLoadFromFile", layerName)
    )
  );
  layer->assetTiledMapDestroy = (
    reinterpret_cast<decltype(layer->assetTiledMapDestroy)>(
      ::loadSymbol("puleAssetTiledMapDestroy", layerName)
    )
  );
  // asset-font
  layer->assetFontLoad = (
    reinterpret_cast<decltype(layer->assetFontLoad)>(
      ::loadSymbol("puleAssetFontLoad", layerName)
    )
  );
  layer->assetFontDestroy = (
    reinterpret_cast<decltype(layer->assetFontDestroy)>(
      ::loadSymbol("puleAssetFontDestroy", layerName)
    )
  );
  layer->assetFontRenderToU8Buffer = (
    reinterpret_cast<decltype(layer->assetFontRenderToU8Buffer)>(
      ::loadSymbol("puleAssetFontRenderToU8Buffer", layerName)
    )
  );
  // asset-pds
  layer->assetPdsLoadFromStream = (
    reinterpret_cast<decltype(layer->assetPdsLoadFromStream)>(
      ::loadSymbol("puleAssetPdsLoadFromStream", layerName)
    )
  );
  layer->assetPdsLoadFromRvalStream = (
    reinterpret_cast<decltype(layer->assetPdsLoadFromRvalStream)>(
      ::loadSymbol("puleAssetPdsLoadFromRvalStream", layerName)
    )
  );
  layer->assetPdsLoadFromFile = (
    reinterpret_cast<decltype(layer->assetPdsLoadFromFile)>(
      ::loadSymbol("puleAssetPdsLoadFromFile", layerName)
    )
  );
  layer->assetPdsLoadFromString = (
    reinterpret_cast<decltype(layer->assetPdsLoadFromString)>(
      ::loadSymbol("puleAssetPdsLoadFromString", layerName)
    )
  );
  layer->assetPdsWriteToStream = (
    reinterpret_cast<decltype(layer->assetPdsWriteToStream)>(
      ::loadSymbol("puleAssetPdsWriteToStream", layerName)
    )
  );
  layer->assetPdsWriteToFile = (
    reinterpret_cast<decltype(layer->assetPdsWriteToFile)>(
      ::loadSymbol("puleAssetPdsWriteToFile", layerName)
    )
  );
  layer->assetPdsWriteToStdout = (
    reinterpret_cast<decltype(layer->assetPdsWriteToStdout)>(
      ::loadSymbol("puleAssetPdsWriteToStdout", layerName)
    )
  );
  layer->assetPdsLoadFromCommandLineArguments = (
    reinterpret_cast<decltype(layer->assetPdsLoadFromCommandLineArguments)>(
      ::loadSymbol("puleAssetPdsLoadFromCommandLineArguments", layerName)
    )
  );
  // asset-shader-module
  layer->assetShaderModuleCreateFromPaths = (
    reinterpret_cast<decltype(layer->assetShaderModuleCreateFromPaths)>(
      ::loadSymbol("puleAssetShaderModuleCreateFromPaths", layerName)
    )
  );
  layer->assetShaderModuleDestroy = (
    reinterpret_cast<decltype(layer->assetShaderModuleDestroy)>(
      ::loadSymbol("puleAssetShaderModuleDestroy", layerName)
    )
  );
  layer->assetShaderModuleLabel = (
    reinterpret_cast<decltype(layer->assetShaderModuleLabel)>(
      ::loadSymbol("puleAssetShaderModuleLabel", layerName)
    )
  );
  layer->assetShaderModuleHandle = (
    reinterpret_cast<decltype(layer->assetShaderModuleHandle)>(
      ::loadSymbol("puleAssetShaderModuleHandle", layerName)
    )
  );
  // asset-render-graph
  layer->assetRenderGraphFromPds = (
    reinterpret_cast<decltype(layer->assetRenderGraphFromPds)>(
      ::loadSymbol("puleAssetRenderGraphFromPds", layerName)
    )
  );
  // camera
  layer->cameraCreate = (
    reinterpret_cast<decltype(layer->cameraCreate)>(
      ::loadSymbol("puleCameraCreate", layerName)
    )
  );
  layer->cameraDestroy = (
    reinterpret_cast<decltype(layer->cameraDestroy)>(
      ::loadSymbol("puleCameraDestroy", layerName)
    )
  );
  layer->cameraView = (
    reinterpret_cast<decltype(layer->cameraView)>(
      ::loadSymbol("puleCameraView", layerName)
    )
  );
  layer->cameraProj = (
    reinterpret_cast<decltype(layer->cameraProj)>(
      ::loadSymbol("puleCameraProj", layerName)
    )
  );
  layer->cameraLookAt = (
    reinterpret_cast<decltype(layer->cameraLookAt)>(
      ::loadSymbol("puleCameraLookAt", layerName)
    )
  );
  layer->cameraPerspective = (
    reinterpret_cast<decltype(layer->cameraPerspective)>(
      ::loadSymbol("puleCameraPerspective", layerName)
    )
  );
  layer->cameraPerspectiveSet = (
    reinterpret_cast<decltype(layer->cameraPerspectiveSet)>(
      ::loadSymbol("puleCameraPerspectiveSet", layerName)
    )
  );
  layer->cameraSetCreate = (
    reinterpret_cast<decltype(layer->cameraSetCreate)>(
      ::loadSymbol("puleCameraSetCreate", layerName)
    )
  );
  layer->cameraSetDestroy = (
    reinterpret_cast<decltype(layer->cameraSetDestroy)>(
      ::loadSymbol("puleCameraSetDestroy", layerName)
    )
  );
  layer->cameraSetAdd = (
    reinterpret_cast<decltype(layer->cameraSetAdd)>(
      ::loadSymbol("puleCameraSetAdd", layerName)
    )
  );
  layer->cameraSetRemove = (
    reinterpret_cast<decltype(layer->cameraSetRemove)>(
      ::loadSymbol("puleCameraSetRemove", layerName)
    )
  );
  layer->cameraSetArray = (
    reinterpret_cast<decltype(layer->cameraSetArray)>(
      ::loadSymbol("puleCameraSetArray", layerName)
    )
  );
  layer->cameraSetUniformBuffer = (
    reinterpret_cast<decltype(layer->cameraSetUniformBuffer)>(
      ::loadSymbol("puleCameraSetUniformBuffer", layerName)
    )
  );
  layer->cameraSetRefresh = (
    reinterpret_cast<decltype(layer->cameraSetRefresh)>(
      ::loadSymbol("puleCameraSetRefresh", layerName)
    )
  );
  layer->cameraControllerFirstPerson = (
    reinterpret_cast<decltype(layer->cameraControllerFirstPerson)>(
      ::loadSymbol("puleCameraControllerFirstPerson", layerName)
    )
  );
  layer->cameraControllerDestroy = (
    reinterpret_cast<decltype(layer->cameraControllerDestroy)>(
      ::loadSymbol("puleCameraControllerDestroy", layerName)
    )
  );
  layer->cameraControllerPollEvents = (
    reinterpret_cast<decltype(layer->cameraControllerPollEvents)>(
      ::loadSymbol("puleCameraControllerPollEvents", layerName)
    )
  );
  // data-serializer
  layer->dsAsI64 = (
    reinterpret_cast<decltype(layer->dsAsI64)>(
      ::loadSymbol("puleDsAsI64", layerName)
    )
  );
  layer->dsAsF64 = (
    reinterpret_cast<decltype(layer->dsAsF64)>(
      ::loadSymbol("puleDsAsF64", layerName)
    )
  );
  layer->dsAsBool = (
    reinterpret_cast<decltype(layer->dsAsBool)>(
      ::loadSymbol("puleDsAsBool", layerName)
    )
  );
  layer->dsAsString = (
    reinterpret_cast<decltype(layer->dsAsString)>(
      ::loadSymbol("puleDsAsString", layerName)
    )
  );
  layer->dsAsArray = (
    reinterpret_cast<decltype(layer->dsAsArray)>(
      ::loadSymbol("puleDsAsArray", layerName)
    )
  );
  layer->dsAsObject = (
    reinterpret_cast<decltype(layer->dsAsObject)>(
      ::loadSymbol("puleDsAsObject", layerName)
    )
  );
  layer->dsAsBuffer = (
    reinterpret_cast<decltype(layer->dsAsBuffer)>(
      ::loadSymbol("puleDsAsBuffer", layerName)
    )
  );
  layer->dsAsF32 = (
    reinterpret_cast<decltype(layer->dsAsF32)>(
      ::loadSymbol("puleDsAsF32", layerName)
    )
  );
  layer->dsAsI32 = (
    reinterpret_cast<decltype(layer->dsAsI32)>(
      ::loadSymbol("puleDsAsI32", layerName)
    )
  );
  layer->dsAsUSize = (
    reinterpret_cast<decltype(layer->dsAsUSize)>(
      ::loadSymbol("puleDsAsUSize", layerName)
    )
  );
  layer->dsAsU64 = (
    reinterpret_cast<decltype(layer->dsAsU64)>(
      ::loadSymbol("puleDsAsU64", layerName)
    )
  );
  layer->dsAsU32 = (
    reinterpret_cast<decltype(layer->dsAsU32)>(
      ::loadSymbol("puleDsAsU32", layerName)
    )
  );
  layer->dsAsU16 = (
    reinterpret_cast<decltype(layer->dsAsU16)>(
      ::loadSymbol("puleDsAsU16", layerName)
    )
  );
  layer->dsAsI16 = (
    reinterpret_cast<decltype(layer->dsAsI16)>(
      ::loadSymbol("puleDsAsI16", layerName)
    )
  );
  layer->dsAsU8 = (
    reinterpret_cast<decltype(layer->dsAsU8)>(
      ::loadSymbol("puleDsAsU8", layerName)
    )
  );
  layer->dsAsI8 = (
    reinterpret_cast<decltype(layer->dsAsI8)>(
      ::loadSymbol("puleDsAsI8", layerName)
    )
  );
  layer->dsIsI64 = (
    reinterpret_cast<decltype(layer->dsIsI64)>(
      ::loadSymbol("puleDsIsI64", layerName)
    )
  );
  layer->dsIsF64 = (
    reinterpret_cast<decltype(layer->dsIsF64)>(
      ::loadSymbol("puleDsIsF64", layerName)
    )
  );
  layer->dsIsString = (
    reinterpret_cast<decltype(layer->dsIsString)>(
      ::loadSymbol("puleDsIsString", layerName)
    )
  );
  layer->dsIsArray = (
    reinterpret_cast<decltype(layer->dsIsArray)>(
      ::loadSymbol("puleDsIsArray", layerName)
    )
  );
  layer->dsIsObject = (
    reinterpret_cast<decltype(layer->dsIsObject)>(
      ::loadSymbol("puleDsIsObject", layerName)
    )
  );
  layer->dsIsBuffer = (
    reinterpret_cast<decltype(layer->dsIsBuffer)>(
      ::loadSymbol("puleDsIsBuffer", layerName)
    )
  );
  layer->dsDestroy = (
    reinterpret_cast<decltype(layer->dsDestroy)>(
      ::loadSymbol("puleDsDestroy", layerName)
    )
  );
  layer->dsCreateI64 = (
    reinterpret_cast<decltype(layer->dsCreateI64)>(
      ::loadSymbol("puleDsCreateI64", layerName)
    )
  );
  layer->dsCreateBool = (
    reinterpret_cast<decltype(layer->dsCreateBool)>(
      ::loadSymbol("puleDsCreateBool", layerName)
    )
  );
  layer->dsCreateU64 = (
    reinterpret_cast<decltype(layer->dsCreateU64)>(
      ::loadSymbol("puleDsCreateU64", layerName)
    )
  );
  layer->dsCreateF64 = (
    reinterpret_cast<decltype(layer->dsCreateF64)>(
      ::loadSymbol("puleDsCreateF64", layerName)
    )
  );
  layer->dsCreateString = (
    reinterpret_cast<decltype(layer->dsCreateString)>(
      ::loadSymbol("puleDsCreateString", layerName)
    )
  );
  layer->dsCreateArray = (
    reinterpret_cast<decltype(layer->dsCreateArray)>(
      ::loadSymbol("puleDsCreateArray", layerName)
    )
  );
  layer->dsCreateObject = (
    reinterpret_cast<decltype(layer->dsCreateObject)>(
      ::loadSymbol("puleDsCreateObject", layerName)
    )
  );
  layer->dsCreateBuffer = (
    reinterpret_cast<decltype(layer->dsCreateBuffer)>(
      ::loadSymbol("puleDsCreateBuffer", layerName)
    )
  );
  layer->dsArrayAppend = (
    reinterpret_cast<decltype(layer->dsArrayAppend)>(
      ::loadSymbol("puleDsArrayAppend", layerName)
    )
  );
  layer->dsArrayPopBack = (
    reinterpret_cast<decltype(layer->dsArrayPopBack)>(
      ::loadSymbol("puleDsArrayPopBack", layerName)
    )
  );
  layer->dsArrayPopFront = (
    reinterpret_cast<decltype(layer->dsArrayPopFront)>(
      ::loadSymbol("puleDsArrayPopFront", layerName)
    )
  );
  layer->dsArrayRemoveAt = (
    reinterpret_cast<decltype(layer->dsArrayRemoveAt)>(
      ::loadSymbol("puleDsArrayRemoveAt", layerName)
    )
  );
  layer->dsArrayLength = (
    reinterpret_cast<decltype(layer->dsArrayLength)>(
      ::loadSymbol("puleDsArrayLength", layerName)
    )
  );
  layer->dsArrayElementAt = (
    reinterpret_cast<decltype(layer->dsArrayElementAt)>(
      ::loadSymbol("puleDsArrayElementAt", layerName)
    )
  );
  layer->dsObjectMember = (
    reinterpret_cast<decltype(layer->dsObjectMember)>(
      ::loadSymbol("puleDsObjectMember", layerName)
    )
  );
  layer->dsIsNull = (
    reinterpret_cast<decltype(layer->dsIsNull)>(
      ::loadSymbol("puleDsIsNull", layerName)
    )
  );
  layer->dsValueCloneRecursively = (
    reinterpret_cast<decltype(layer->dsValueCloneRecursively)>(
      ::loadSymbol("puleDsValueCloneRecursively", layerName)
    )
  );
  layer->dsObjectMemberAssign = (
    reinterpret_cast<decltype(layer->dsObjectMemberAssign)>(
      ::loadSymbol("puleDsObjectMemberAssign", layerName)
    )
  );
  layer->dsObjectMemberOverwrite = (
    reinterpret_cast<decltype(layer->dsObjectMemberOverwrite)>(
      ::loadSymbol("puleDsObjectMemberOverwrite", layerName)
    )
  );
  layer->dsMemberAsI64 = (
    reinterpret_cast<decltype(layer->dsMemberAsI64)>(
      ::loadSymbol("puleDsMemberAsI64", layerName)
    )
  );
  layer->dsMemberAsF64 = (
    reinterpret_cast<decltype(layer->dsMemberAsF64)>(
      ::loadSymbol("puleDsMemberAsF64", layerName)
    )
  );
  layer->dsMemberAsF32 = (
    reinterpret_cast<decltype(layer->dsMemberAsF32)>(
      ::loadSymbol("puleDsMemberAsF32", layerName)
    )
  );
  layer->dsMemberAsBool = (
    reinterpret_cast<decltype(layer->dsMemberAsBool)>(
      ::loadSymbol("puleDsMemberAsBool", layerName)
    )
  );
  layer->dsMemberAsString = (
    reinterpret_cast<decltype(layer->dsMemberAsString)>(
      ::loadSymbol("puleDsMemberAsString", layerName)
    )
  );
  layer->dsMemberAsArray = (
    reinterpret_cast<decltype(layer->dsMemberAsArray)>(
      ::loadSymbol("puleDsMemberAsArray", layerName)
    )
  );
  layer->dsMemberAsObject = (
    reinterpret_cast<decltype(layer->dsMemberAsObject)>(
      ::loadSymbol("puleDsMemberAsObject", layerName)
    )
  );
  layer->dsMemberAsBuffer = (
    reinterpret_cast<decltype(layer->dsMemberAsBuffer)>(
      ::loadSymbol("puleDsMemberAsBuffer", layerName)
    )
  );
  // ecs-serializer
  layer->ecsSerializeWorld = (
    reinterpret_cast<decltype(layer->ecsSerializeWorld)>(
      ::loadSymbol("puleEcsSerializeWorld", layerName)
    )
  );
  layer->ecsDeserializeWorld = (
    reinterpret_cast<decltype(layer->ecsDeserializeWorld)>(
      ::loadSymbol("puleEcsDeserializeWorld", layerName)
    )
  );
  // ecs
  layer->ecsWorldCreate = (
    reinterpret_cast<decltype(layer->ecsWorldCreate)>(
      ::loadSymbol("puleEcsWorldCreate", layerName)
    )
  );
  layer->ecsWorldDestroy = (
    reinterpret_cast<decltype(layer->ecsWorldDestroy)>(
      ::loadSymbol("puleEcsWorldDestroy", layerName)
    )
  );
  layer->ecsWorldAdvance = (
    reinterpret_cast<decltype(layer->ecsWorldAdvance)>(
      ::loadSymbol("puleEcsWorldAdvance", layerName)
    )
  );
  layer->ecsComponentCreate = (
    reinterpret_cast<decltype(layer->ecsComponentCreate)>(
      ::loadSymbol("puleEcsComponentCreate", layerName)
    )
  );
  layer->ecsEntityIterateComponents = (
    reinterpret_cast<decltype(layer->ecsEntityIterateComponents)>(
      ::loadSymbol("puleEcsEntityIterateComponents", layerName)
    )
  );
  layer->ecsComponentFetchByLabel = (
    reinterpret_cast<decltype(layer->ecsComponentFetchByLabel)>(
      ::loadSymbol("puleEcsComponentFetchByLabel", layerName)
    )
  );
  layer->ecsComponentLabel = (
    reinterpret_cast<decltype(layer->ecsComponentLabel)>(
      ::loadSymbol("puleEcsComponentLabel", layerName)
    )
  );
  layer->ecsComponentSerializer = (
    reinterpret_cast<decltype(layer->ecsComponentSerializer)>(
      ::loadSymbol("puleEcsComponentSerializer", layerName)
    )
  );
  layer->ecsComponentInfo = (
    reinterpret_cast<decltype(layer->ecsComponentInfo)>(
      ::loadSymbol("puleEcsComponentInfo", layerName)
    )
  );
  layer->ecsComponentIterateAll = (
    reinterpret_cast<decltype(layer->ecsComponentIterateAll)>(
      ::loadSymbol("puleEcsComponentIterateAll", layerName)
    )
  );
  layer->ecsIteratorEntityCount = (
    reinterpret_cast<decltype(layer->ecsIteratorEntityCount)>(
      ::loadSymbol("puleEcsIteratorEntityCount", layerName)
    )
  );
  layer->ecsIteratorRelativeOffset = (
    reinterpret_cast<decltype(layer->ecsIteratorRelativeOffset)>(
      ::loadSymbol("puleEcsIteratorRelativeOffset", layerName)
    )
  );
  layer->ecsIteratorQueryComponents = (
    reinterpret_cast<decltype(layer->ecsIteratorQueryComponents)>(
      ::loadSymbol("puleEcsIteratorQueryComponents", layerName)
    )
  );
  layer->ecsIteratorQueryEntities = (
    reinterpret_cast<decltype(layer->ecsIteratorQueryEntities)>(
      ::loadSymbol("puleEcsIteratorQueryEntities", layerName)
    )
  );
  layer->ecsIteratorWorld = (
    reinterpret_cast<decltype(layer->ecsIteratorWorld)>(
      ::loadSymbol("puleEcsIteratorWorld", layerName)
    )
  );
  layer->ecsIteratorSystem = (
    reinterpret_cast<decltype(layer->ecsIteratorSystem)>(
      ::loadSymbol("puleEcsIteratorSystem", layerName)
    )
  );
  layer->ecsIteratorUserData = (
    reinterpret_cast<decltype(layer->ecsIteratorUserData)>(
      ::loadSymbol("puleEcsIteratorUserData", layerName)
    )
  );
  layer->ecsSystemCreate = (
    reinterpret_cast<decltype(layer->ecsSystemCreate)>(
      ::loadSymbol("puleEcsSystemCreate", layerName)
    )
  );
  layer->ecsSystemAdvance = (
    reinterpret_cast<decltype(layer->ecsSystemAdvance)>(
      ::loadSymbol("puleEcsSystemAdvance", layerName)
    )
  );
  layer->ecsEntityCreate = (
    reinterpret_cast<decltype(layer->ecsEntityCreate)>(
      ::loadSymbol("puleEcsEntityCreate", layerName)
    )
  );
  layer->ecsEntityName = (
    reinterpret_cast<decltype(layer->ecsEntityName)>(
      ::loadSymbol("puleEcsEntityName", layerName)
    )
  );
  layer->ecsEntityDestroy = (
    reinterpret_cast<decltype(layer->ecsEntityDestroy)>(
      ::loadSymbol("puleEcsEntityDestroy", layerName)
    )
  );
  layer->ecsEntityAttachComponent = (
    reinterpret_cast<decltype(layer->ecsEntityAttachComponent)>(
      ::loadSymbol("puleEcsEntityAttachComponent", layerName)
    )
  );
  layer->ecsEntityComponentData = (
    reinterpret_cast<decltype(layer->ecsEntityComponentData)>(
      ::loadSymbol("puleEcsEntityComponentData", layerName)
    )
  );
  layer->ecsQueryByComponent = (
    reinterpret_cast<decltype(layer->ecsQueryByComponent)>(
      ::loadSymbol("puleEcsQueryByComponent", layerName)
    )
  );
  layer->ecsQueryAllEntities = (
    reinterpret_cast<decltype(layer->ecsQueryAllEntities)>(
      ::loadSymbol("puleEcsQueryAllEntities", layerName)
    )
  );
  layer->ecsQueryDestroy = (
    reinterpret_cast<decltype(layer->ecsQueryDestroy)>(
      ::loadSymbol("puleEcsQueryDestroy", layerName)
    )
  );
  layer->ecsQueryIterator = (
    reinterpret_cast<decltype(layer->ecsQueryIterator)>(
      ::loadSymbol("puleEcsQueryIterator", layerName)
    )
  );
  layer->ecsQueryIteratorNext = (
    reinterpret_cast<decltype(layer->ecsQueryIteratorNext)>(
      ::loadSymbol("puleEcsQueryIteratorNext", layerName)
    )
  );
  layer->ecsQueryIteratorDestroy = (
    reinterpret_cast<decltype(layer->ecsQueryIteratorDestroy)>(
      ::loadSymbol("puleEcsQueryIteratorDestroy", layerName)
    )
  );
  // error
  layer->error = (
    reinterpret_cast<decltype(layer->error)>(
      ::loadSymbol("puleError", layerName)
    )
  );
  layer->errorConsume = (
    reinterpret_cast<decltype(layer->errorConsume)>(
      ::loadSymbol("puleErrorConsume", layerName)
    )
  );
  layer->errorExists = (
    reinterpret_cast<decltype(layer->errorExists)>(
      ::loadSymbol("puleErrorExists", layerName)
    )
  );
  // file
  layer->fileOpen = (
    reinterpret_cast<decltype(layer->fileOpen)>(
      ::loadSymbol("puleFileOpen", layerName)
    )
  );
  layer->fileClose = (
    reinterpret_cast<decltype(layer->fileClose)>(
      ::loadSymbol("puleFileClose", layerName)
    )
  );
  layer->fileIsDone = (
    reinterpret_cast<decltype(layer->fileIsDone)>(
      ::loadSymbol("puleFileIsDone", layerName)
    )
  );
  layer->filePath = (
    reinterpret_cast<decltype(layer->filePath)>(
      ::loadSymbol("puleFilePath", layerName)
    )
  );
  layer->fileReadByte = (
    reinterpret_cast<decltype(layer->fileReadByte)>(
      ::loadSymbol("puleFileReadByte", layerName)
    )
  );
  layer->fileReadBytes = (
    reinterpret_cast<decltype(layer->fileReadBytes)>(
      ::loadSymbol("puleFileReadBytes", layerName)
    )
  );
  layer->fileWriteBytes = (
    reinterpret_cast<decltype(layer->fileWriteBytes)>(
      ::loadSymbol("puleFileWriteBytes", layerName)
    )
  );
  layer->fileWriteString = (
    reinterpret_cast<decltype(layer->fileWriteString)>(
      ::loadSymbol("puleFileWriteString", layerName)
    )
  );
  layer->fileSize = (
    reinterpret_cast<decltype(layer->fileSize)>(
      ::loadSymbol("puleFileSize", layerName)
    )
  );
  layer->fileAdvanceFromStart = (
    reinterpret_cast<decltype(layer->fileAdvanceFromStart)>(
      ::loadSymbol("puleFileAdvanceFromStart", layerName)
    )
  );
  layer->fileAdvanceFromEnd = (
    reinterpret_cast<decltype(layer->fileAdvanceFromEnd)>(
      ::loadSymbol("puleFileAdvanceFromEnd", layerName)
    )
  );
  layer->fileAdvanceFromCurrent = (
    reinterpret_cast<decltype(layer->fileAdvanceFromCurrent)>(
      ::loadSymbol("puleFileAdvanceFromCurrent", layerName)
    )
  );
  layer->fileStreamRead = (
    reinterpret_cast<decltype(layer->fileStreamRead)>(
      ::loadSymbol("puleFileStreamRead", layerName)
    )
  );
  layer->fileStreamWrite = (
    reinterpret_cast<decltype(layer->fileStreamWrite)>(
      ::loadSymbol("puleFileStreamWrite", layerName)
    )
  );
  layer->filesystemPathExists = (
    reinterpret_cast<decltype(layer->filesystemPathExists)>(
      ::loadSymbol("puleFilesystemPathExists", layerName)
    )
  );
  layer->fileCopy = (
    reinterpret_cast<decltype(layer->fileCopy)>(
      ::loadSymbol("puleFileCopy", layerName)
    )
  );
  layer->fileRemove = (
    reinterpret_cast<decltype(layer->fileRemove)>(
      ::loadSymbol("puleFileRemove", layerName)
    )
  );
  layer->fileRemoveRecursive = (
    reinterpret_cast<decltype(layer->fileRemoveRecursive)>(
      ::loadSymbol("puleFileRemoveRecursive", layerName)
    )
  );
  layer->fileDirectoryCreate = (
    reinterpret_cast<decltype(layer->fileDirectoryCreate)>(
      ::loadSymbol("puleFileDirectoryCreate", layerName)
    )
  );
  layer->fileDirectoryCreateRecursive = (
    reinterpret_cast<decltype(layer->fileDirectoryCreateRecursive)>(
      ::loadSymbol("puleFileDirectoryCreateRecursive", layerName)
    )
  );
  layer->filesystemExecutablePath = (
    reinterpret_cast<decltype(layer->filesystemExecutablePath)>(
      ::loadSymbol("puleFilesystemExecutablePath", layerName)
    )
  );
  layer->filesystemCurrentPath = (
    reinterpret_cast<decltype(layer->filesystemCurrentPath)>(
      ::loadSymbol("puleFilesystemCurrentPath", layerName)
    )
  );
  layer->filesystemAssetPath = (
    reinterpret_cast<decltype(layer->filesystemAssetPath)>(
      ::loadSymbol("puleFilesystemAssetPath", layerName)
    )
  );
  layer->filesystemAssetPathSet = (
    reinterpret_cast<decltype(layer->filesystemAssetPathSet)>(
      ::loadSymbol("puleFilesystemAssetPathSet", layerName)
    )
  );
  layer->filesystemAbsolutePath = (
    reinterpret_cast<decltype(layer->filesystemAbsolutePath)>(
      ::loadSymbol("puleFilesystemAbsolutePath", layerName)
    )
  );
  layer->filesystemSymlinkCreate = (
    reinterpret_cast<decltype(layer->filesystemSymlinkCreate)>(
      ::loadSymbol("puleFilesystemSymlinkCreate", layerName)
    )
  );
  layer->filesystemTimestamp = (
    reinterpret_cast<decltype(layer->filesystemTimestamp)>(
      ::loadSymbol("puleFilesystemTimestamp", layerName)
    )
  );
  layer->fileWatch = (
    reinterpret_cast<decltype(layer->fileWatch)>(
      ::loadSymbol("puleFileWatch", layerName)
    )
  );
  layer->fileWatchCheckAll = (
    reinterpret_cast<decltype(layer->fileWatchCheckAll)>(
      ::loadSymbol("puleFileWatchCheckAll", layerName)
    )
  );
  // gfx-debug
  layer->gfxDebugInitialize = (
    reinterpret_cast<decltype(layer->gfxDebugInitialize)>(
      ::loadSymbol("puleGfxDebugInitialize", layerName)
    )
  );
  layer->gfxDebugShutdown = (
    reinterpret_cast<decltype(layer->gfxDebugShutdown)>(
      ::loadSymbol("puleGfxDebugShutdown", layerName)
    )
  );
  layer->gfxDebugStart = (
    reinterpret_cast<decltype(layer->gfxDebugStart)>(
      ::loadSymbol("puleGfxDebugStart", layerName)
    )
  );
  layer->gfxDebugEnd = (
    reinterpret_cast<decltype(layer->gfxDebugEnd)>(
      ::loadSymbol("puleGfxDebugEnd", layerName)
    )
  );
  layer->gfxDebugRender = (
    reinterpret_cast<decltype(layer->gfxDebugRender)>(
      ::loadSymbol("puleGfxDebugRender", layerName)
    )
  );
  // gfx-mesh
  layer->gfxMeshRender = (
    reinterpret_cast<decltype(layer->gfxMeshRender)>(
      ::loadSymbol("puleGfxMeshRender", layerName)
    )
  );
  // imgui-engine
  layer->imguiEngineDisplay = (
    reinterpret_cast<decltype(layer->imguiEngineDisplay)>(
      ::loadSymbol("puleImguiEngineDisplay", layerName)
    )
  );
  // imgui
  layer->imguiInitialize = (
    reinterpret_cast<decltype(layer->imguiInitialize)>(
      ::loadSymbol("puleImguiInitialize", layerName)
    )
  );
  layer->imguiShutdown = (
    reinterpret_cast<decltype(layer->imguiShutdown)>(
      ::loadSymbol("puleImguiShutdown", layerName)
    )
  );
  layer->imguiNewFrame = (
    reinterpret_cast<decltype(layer->imguiNewFrame)>(
      ::loadSymbol("puleImguiNewFrame", layerName)
    )
  );
  layer->imguiRender = (
    reinterpret_cast<decltype(layer->imguiRender)>(
      ::loadSymbol("puleImguiRender", layerName)
    )
  );
  layer->imguiJoinNext = (
    reinterpret_cast<decltype(layer->imguiJoinNext)>(
      ::loadSymbol("puleImguiJoinNext", layerName)
    )
  );
  layer->imguiSliderF32 = (
    reinterpret_cast<decltype(layer->imguiSliderF32)>(
      ::loadSymbol("puleImguiSliderF32", layerName)
    )
  );
  layer->imguiSliderZu = (
    reinterpret_cast<decltype(layer->imguiSliderZu)>(
      ::loadSymbol("puleImguiSliderZu", layerName)
    )
  );
  layer->imguiWindowBegin = (
    reinterpret_cast<decltype(layer->imguiWindowBegin)>(
      ::loadSymbol("puleImguiWindowBegin", layerName)
    )
  );
  layer->imguiWindowEnd = (
    reinterpret_cast<decltype(layer->imguiWindowEnd)>(
      ::loadSymbol("puleImguiWindowEnd", layerName)
    )
  );
  layer->imguiSectionBegin = (
    reinterpret_cast<decltype(layer->imguiSectionBegin)>(
      ::loadSymbol("puleImguiSectionBegin", layerName)
    )
  );
  layer->imguiSectionEnd = (
    reinterpret_cast<decltype(layer->imguiSectionEnd)>(
      ::loadSymbol("puleImguiSectionEnd", layerName)
    )
  );
  layer->imguiText = (
    reinterpret_cast<decltype(layer->imguiText)>(
      ::loadSymbol("puleImguiText", layerName)
    )
  );
  layer->imguiImage = (
    reinterpret_cast<decltype(layer->imguiImage)>(
      ::loadSymbol("puleImguiImage", layerName)
    )
  );
  layer->imguiLastItemHovered = (
    reinterpret_cast<decltype(layer->imguiLastItemHovered)>(
      ::loadSymbol("puleImguiLastItemHovered", layerName)
    )
  );
  layer->imguiCurrentOrigin = (
    reinterpret_cast<decltype(layer->imguiCurrentOrigin)>(
      ::loadSymbol("puleImguiCurrentOrigin", layerName)
    )
  );
  layer->imguiToggle = (
    reinterpret_cast<decltype(layer->imguiToggle)>(
      ::loadSymbol("puleImguiToggle", layerName)
    )
  );
  layer->imguiButton = (
    reinterpret_cast<decltype(layer->imguiButton)>(
      ::loadSymbol("puleImguiButton", layerName)
    )
  );
  layer->imguiCallbackRegister = (
    reinterpret_cast<decltype(layer->imguiCallbackRegister)>(
      ::loadSymbol("puleImguiCallbackRegister", layerName)
    )
  );
  layer->imguiCallbackUnregister = (
    reinterpret_cast<decltype(layer->imguiCallbackUnregister)>(
      ::loadSymbol("puleImguiCallbackUnregister", layerName)
    )
  );
  layer->imguiCallbackShowAll = (
    reinterpret_cast<decltype(layer->imguiCallbackShowAll)>(
      ::loadSymbol("puleImguiCallbackShowAll", layerName)
    )
  );
  // log
  layer->logDebugEnabled = (
    reinterpret_cast<decltype(layer->logDebugEnabled)>(
      ::loadSymbol("puleLogDebugEnabled", layerName)
    )
  );
  layer->logErrorSegfaultsEnabled = (
    reinterpret_cast<decltype(layer->logErrorSegfaultsEnabled)>(
      ::loadSymbol("puleLogErrorSegfaultsEnabled", layerName)
    )
  );
  layer->log = (
    reinterpret_cast<decltype(layer->log)>(
      ::loadSymbol("puleLog", layerName)
    )
  );
  layer->logDebug = (
    reinterpret_cast<decltype(layer->logDebug)>(
      ::loadSymbol("puleLogDebug", layerName)
    )
  );
  layer->logWarn = (
    reinterpret_cast<decltype(layer->logWarn)>(
      ::loadSymbol("puleLogWarn", layerName)
    )
  );
  layer->logError = (
    reinterpret_cast<decltype(layer->logError)>(
      ::loadSymbol("puleLogError", layerName)
    )
  );
  layer->logLn = (
    reinterpret_cast<decltype(layer->logLn)>(
      ::loadSymbol("puleLogLn", layerName)
    )
  );
  layer->logRaw = (
    reinterpret_cast<decltype(layer->logRaw)>(
      ::loadSymbol("puleLogRaw", layerName)
    )
  );
  // math
  layer->f32v2 = (
    reinterpret_cast<decltype(layer->f32v2)>(
      ::loadSymbol("puleF32v2", layerName)
    )
  );
  layer->f32v2Ptr = (
    reinterpret_cast<decltype(layer->f32v2Ptr)>(
      ::loadSymbol("puleF32v2Ptr", layerName)
    )
  );
  layer->f32v2Add = (
    reinterpret_cast<decltype(layer->f32v2Add)>(
      ::loadSymbol("puleF32v2Add", layerName)
    )
  );
  layer->f32v2Sub = (
    reinterpret_cast<decltype(layer->f32v2Sub)>(
      ::loadSymbol("puleF32v2Sub", layerName)
    )
  );
  layer->f32v2Abs = (
    reinterpret_cast<decltype(layer->f32v2Abs)>(
      ::loadSymbol("puleF32v2Abs", layerName)
    )
  );
  layer->i32v2 = (
    reinterpret_cast<decltype(layer->i32v2)>(
      ::loadSymbol("puleI32v2", layerName)
    )
  );
  layer->i32v2Ptr = (
    reinterpret_cast<decltype(layer->i32v2Ptr)>(
      ::loadSymbol("puleI32v2Ptr", layerName)
    )
  );
  layer->i32v2Add = (
    reinterpret_cast<decltype(layer->i32v2Add)>(
      ::loadSymbol("puleI32v2Add", layerName)
    )
  );
  layer->i32v2Sub = (
    reinterpret_cast<decltype(layer->i32v2Sub)>(
      ::loadSymbol("puleI32v2Sub", layerName)
    )
  );
  layer->i32v2Abs = (
    reinterpret_cast<decltype(layer->i32v2Abs)>(
      ::loadSymbol("puleI32v2Abs", layerName)
    )
  );
  layer->f32v3 = (
    reinterpret_cast<decltype(layer->f32v3)>(
      ::loadSymbol("puleF32v3", layerName)
    )
  );
  layer->f32v3Ptr = (
    reinterpret_cast<decltype(layer->f32v3Ptr)>(
      ::loadSymbol("puleF32v3Ptr", layerName)
    )
  );
  layer->f32v3Add = (
    reinterpret_cast<decltype(layer->f32v3Add)>(
      ::loadSymbol("puleF32v3Add", layerName)
    )
  );
  layer->f32v3Sub = (
    reinterpret_cast<decltype(layer->f32v3Sub)>(
      ::loadSymbol("puleF32v3Sub", layerName)
    )
  );
  layer->f32v3Neg = (
    reinterpret_cast<decltype(layer->f32v3Neg)>(
      ::loadSymbol("puleF32v3Neg", layerName)
    )
  );
  layer->f32v3Mul = (
    reinterpret_cast<decltype(layer->f32v3Mul)>(
      ::loadSymbol("puleF32v3Mul", layerName)
    )
  );
  layer->f32v3MulScalar = (
    reinterpret_cast<decltype(layer->f32v3MulScalar)>(
      ::loadSymbol("puleF32v3MulScalar", layerName)
    )
  );
  layer->f32v3Div = (
    reinterpret_cast<decltype(layer->f32v3Div)>(
      ::loadSymbol("puleF32v3Div", layerName)
    )
  );
  layer->f32v3Dot = (
    reinterpret_cast<decltype(layer->f32v3Dot)>(
      ::loadSymbol("puleF32v3Dot", layerName)
    )
  );
  layer->f32v3Length = (
    reinterpret_cast<decltype(layer->f32v3Length)>(
      ::loadSymbol("puleF32v3Length", layerName)
    )
  );
  layer->f32v3Normalize = (
    reinterpret_cast<decltype(layer->f32v3Normalize)>(
      ::loadSymbol("puleF32v3Normalize", layerName)
    )
  );
  layer->f32v3Cross = (
    reinterpret_cast<decltype(layer->f32v3Cross)>(
      ::loadSymbol("puleF32v3Cross", layerName)
    )
  );
  layer->f32v4 = (
    reinterpret_cast<decltype(layer->f32v4)>(
      ::loadSymbol("puleF32v4", layerName)
    )
  );
  layer->f32m44 = (
    reinterpret_cast<decltype(layer->f32m44)>(
      ::loadSymbol("puleF32m44", layerName)
    )
  );
  layer->f32m44Ptr = (
    reinterpret_cast<decltype(layer->f32m44Ptr)>(
      ::loadSymbol("puleF32m44Ptr", layerName)
    )
  );
  layer->f32m44PtrTranspose = (
    reinterpret_cast<decltype(layer->f32m44PtrTranspose)>(
      ::loadSymbol("puleF32m44PtrTranspose", layerName)
    )
  );
  layer->f32m44Inverse = (
    reinterpret_cast<decltype(layer->f32m44Inverse)>(
      ::loadSymbol("puleF32m44Inverse", layerName)
    )
  );
  layer->f32m44MulV4 = (
    reinterpret_cast<decltype(layer->f32m44MulV4)>(
      ::loadSymbol("puleF32m44MulV4", layerName)
    )
  );
  layer->f32m44DumpToStdout = (
    reinterpret_cast<decltype(layer->f32m44DumpToStdout)>(
      ::loadSymbol("puleF32m44DumpToStdout", layerName)
    )
  );
  layer->projectionPerspective = (
    reinterpret_cast<decltype(layer->projectionPerspective)>(
      ::loadSymbol("puleProjectionPerspective", layerName)
    )
  );
  layer->viewLookAt = (
    reinterpret_cast<decltype(layer->viewLookAt)>(
      ::loadSymbol("puleViewLookAt", layerName)
    )
  );
  // platform
  layer->platformInitialize = (
    reinterpret_cast<decltype(layer->platformInitialize)>(
      ::loadSymbol("pulePlatformInitialize", layerName)
    )
  );
  layer->platformShutdown = (
    reinterpret_cast<decltype(layer->platformShutdown)>(
      ::loadSymbol("pulePlatformShutdown", layerName)
    )
  );
  layer->platformCreate = (
    reinterpret_cast<decltype(layer->platformCreate)>(
      ::loadSymbol("pulePlatformCreate", layerName)
    )
  );
  layer->platformDestroy = (
    reinterpret_cast<decltype(layer->platformDestroy)>(
      ::loadSymbol("pulePlatformDestroy", layerName)
    )
  );
  layer->platformShouldExit = (
    reinterpret_cast<decltype(layer->platformShouldExit)>(
      ::loadSymbol("pulePlatformShouldExit", layerName)
    )
  );
  layer->platformGetProcessAddress = (
    reinterpret_cast<decltype(layer->platformGetProcessAddress)>(
      ::loadSymbol("pulePlatformGetProcessAddress", layerName)
    )
  );
  layer->platformRequiredExtensions = (
    reinterpret_cast<decltype(layer->platformRequiredExtensions)>(
      ::loadSymbol("pulePlatformRequiredExtensions", layerName)
    )
  );
  layer->platformPollEvents = (
    reinterpret_cast<decltype(layer->platformPollEvents)>(
      ::loadSymbol("pulePlatformPollEvents", layerName)
    )
  );
  layer->platformSwapFramebuffer = (
    reinterpret_cast<decltype(layer->platformSwapFramebuffer)>(
      ::loadSymbol("pulePlatformSwapFramebuffer", layerName)
    )
  );
  layer->platformWindowSize = (
    reinterpret_cast<decltype(layer->platformWindowSize)>(
      ::loadSymbol("pulePlatformWindowSize", layerName)
    )
  );
  layer->platformFramebufferSize = (
    reinterpret_cast<decltype(layer->platformFramebufferSize)>(
      ::loadSymbol("pulePlatformFramebufferSize", layerName)
    )
  );
  layer->platformFramebufferResizeCallback = (
    reinterpret_cast<decltype(layer->platformFramebufferResizeCallback)>(
      ::loadSymbol("pulePlatformFramebufferResizeCallback", layerName)
    )
  );
  layer->platformWindowResizeCallback = (
    reinterpret_cast<decltype(layer->platformWindowResizeCallback)>(
      ::loadSymbol("pulePlatformWindowResizeCallback", layerName)
    )
  );
  layer->platformGetTime = (
    reinterpret_cast<decltype(layer->platformGetTime)>(
      ::loadSymbol("pulePlatformGetTime", layerName)
    )
  );
  layer->platformNull = (
    reinterpret_cast<decltype(layer->platformNull)>(
      ::loadSymbol("pulePlatformNull", layerName)
    )
  );
  layer->platformFocused = (
    reinterpret_cast<decltype(layer->platformFocused)>(
      ::loadSymbol("pulePlatformFocused", layerName)
    )
  );
  layer->platformMouseOriginSet = (
    reinterpret_cast<decltype(layer->platformMouseOriginSet)>(
      ::loadSymbol("pulePlatformMouseOriginSet", layerName)
    )
  );
  layer->platformMouseOrigin = (
    reinterpret_cast<decltype(layer->platformMouseOrigin)>(
      ::loadSymbol("pulePlatformMouseOrigin", layerName)
    )
  );
  layer->platformCursorEnabled = (
    reinterpret_cast<decltype(layer->platformCursorEnabled)>(
      ::loadSymbol("pulePlatformCursorEnabled", layerName)
    )
  );
  layer->platformCursorHide = (
    reinterpret_cast<decltype(layer->platformCursorHide)>(
      ::loadSymbol("pulePlatformCursorHide", layerName)
    )
  );
  layer->platformCursorShow = (
    reinterpret_cast<decltype(layer->platformCursorShow)>(
      ::loadSymbol("pulePlatformCursorShow", layerName)
    )
  );
  layer->inputKey = (
    reinterpret_cast<decltype(layer->inputKey)>(
      ::loadSymbol("puleInputKey", layerName)
    )
  );
  layer->inputKeyModifiers = (
    reinterpret_cast<decltype(layer->inputKeyModifiers)>(
      ::loadSymbol("puleInputKeyModifiers", layerName)
    )
  );
  layer->inputMouse = (
    reinterpret_cast<decltype(layer->inputMouse)>(
      ::loadSymbol("puleInputMouse", layerName)
    )
  );
  layer->inputScroll = (
    reinterpret_cast<decltype(layer->inputScroll)>(
      ::loadSymbol("puleInputScroll", layerName)
    )
  );
  layer->inputKeyCallback = (
    reinterpret_cast<decltype(layer->inputKeyCallback)>(
      ::loadSymbol("puleInputKeyCallback", layerName)
    )
  );
  layer->inputMouseButtonCallback = (
    reinterpret_cast<decltype(layer->inputMouseButtonCallback)>(
      ::loadSymbol("puleInputMouseButtonCallback", layerName)
    )
  );
  layer->inputRawTextCallback = (
    reinterpret_cast<decltype(layer->inputRawTextCallback)>(
      ::loadSymbol("puleInputRawTextCallback", layerName)
    )
  );
  // plugin
  layer->pluginPayloadCreate = (
    reinterpret_cast<decltype(layer->pluginPayloadCreate)>(
      ::loadSymbol("pulePluginPayloadCreate", layerName)
    )
  );
  layer->pluginPayloadDestroy = (
    reinterpret_cast<decltype(layer->pluginPayloadDestroy)>(
      ::loadSymbol("pulePluginPayloadDestroy", layerName)
    )
  );
  layer->pluginPayloadFetch = (
    reinterpret_cast<decltype(layer->pluginPayloadFetch)>(
      ::loadSymbol("pulePluginPayloadFetch", layerName)
    )
  );
  layer->pluginPayloadFetchU64 = (
    reinterpret_cast<decltype(layer->pluginPayloadFetchU64)>(
      ::loadSymbol("pulePluginPayloadFetchU64", layerName)
    )
  );
  layer->pluginPayloadStore = (
    reinterpret_cast<decltype(layer->pluginPayloadStore)>(
      ::loadSymbol("pulePluginPayloadStore", layerName)
    )
  );
  layer->pluginPayloadStoreU64 = (
    reinterpret_cast<decltype(layer->pluginPayloadStoreU64)>(
      ::loadSymbol("pulePluginPayloadStoreU64", layerName)
    )
  );
  layer->pluginPayloadRemove = (
    reinterpret_cast<decltype(layer->pluginPayloadRemove)>(
      ::loadSymbol("pulePluginPayloadRemove", layerName)
    )
  );
  layer->pluginsLoad = (
    reinterpret_cast<decltype(layer->pluginsLoad)>(
      ::loadSymbol("pulePluginsLoad", layerName)
    )
  );
  layer->pluginsFree = (
    reinterpret_cast<decltype(layer->pluginsFree)>(
      ::loadSymbol("pulePluginsFree", layerName)
    )
  );
  layer->pluginsReload = (
    reinterpret_cast<decltype(layer->pluginsReload)>(
      ::loadSymbol("pulePluginsReload", layerName)
    )
  );
  layer->pluginIdFromName = (
    reinterpret_cast<decltype(layer->pluginIdFromName)>(
      ::loadSymbol("pulePluginIdFromName", layerName)
    )
  );
  layer->pluginName = (
    reinterpret_cast<decltype(layer->pluginName)>(
      ::loadSymbol("pulePluginName", layerName)
    )
  );
  layer->pluginLoadFn = (
    reinterpret_cast<decltype(layer->pluginLoadFn)>(
      ::loadSymbol("pulePluginLoadFn", layerName)
    )
  );
  layer->pluginLoadFnTry = (
    reinterpret_cast<decltype(layer->pluginLoadFnTry)>(
      ::loadSymbol("pulePluginLoadFnTry", layerName)
    )
  );
  layer->pluginIterate = (
    reinterpret_cast<decltype(layer->pluginIterate)>(
      ::loadSymbol("pulePluginIterate", layerName)
    )
  );
  // raycast
  layer->raycastTriangles = (
    reinterpret_cast<decltype(layer->raycastTriangles)>(
      ::loadSymbol("puleRaycastTriangles", layerName)
    )
  );
  // renderer-3d
  layer->renderer3DCreate = (
    reinterpret_cast<decltype(layer->renderer3DCreate)>(
      ::loadSymbol("puleRenderer3DCreate", layerName)
    )
  );
  layer->renderer3DEcsSystem = (
    reinterpret_cast<decltype(layer->renderer3DEcsSystem)>(
      ::loadSymbol("puleRenderer3DEcsSystem", layerName)
    )
  );
  layer->renderer3DPrepareModel = (
    reinterpret_cast<decltype(layer->renderer3DPrepareModel)>(
      ::loadSymbol("puleRenderer3DPrepareModel", layerName)
    )
  );
  layer->renderer3DAttachComponentRender = (
    reinterpret_cast<decltype(layer->renderer3DAttachComponentRender)>(
      ::loadSymbol("puleRenderer3DAttachComponentRender", layerName)
    )
  );
  // script
  layer->scriptContextCreate = (
    reinterpret_cast<decltype(layer->scriptContextCreate)>(
      ::loadSymbol("puleScriptContextCreate", layerName)
    )
  );
  layer->scriptContextDestroy = (
    reinterpret_cast<decltype(layer->scriptContextDestroy)>(
      ::loadSymbol("puleScriptContextDestroy", layerName)
    )
  );
  layer->scriptModuleCreateFromSource = (
    reinterpret_cast<decltype(layer->scriptModuleCreateFromSource)>(
      ::loadSymbol("puleScriptModuleCreateFromSource", layerName)
    )
  );
  layer->scriptModuleUpdateFromSource = (
    reinterpret_cast<decltype(layer->scriptModuleUpdateFromSource)>(
      ::loadSymbol("puleScriptModuleUpdateFromSource", layerName)
    )
  );
  layer->scriptModuleCreateFromBinary = (
    reinterpret_cast<decltype(layer->scriptModuleCreateFromBinary)>(
      ::loadSymbol("puleScriptModuleCreateFromBinary", layerName)
    )
  );
  layer->scriptModuleDestroy = (
    reinterpret_cast<decltype(layer->scriptModuleDestroy)>(
      ::loadSymbol("puleScriptModuleDestroy", layerName)
    )
  );
  layer->scriptModuleExecute = (
    reinterpret_cast<decltype(layer->scriptModuleExecute)>(
      ::loadSymbol("puleScriptModuleExecute", layerName)
    )
  );
  layer->scriptGlobal = (
    reinterpret_cast<decltype(layer->scriptGlobal)>(
      ::loadSymbol("puleScriptGlobal", layerName)
    )
  );
  layer->scriptGlobalSet = (
    reinterpret_cast<decltype(layer->scriptGlobalSet)>(
      ::loadSymbol("puleScriptGlobalSet", layerName)
    )
  );
  layer->scriptArrayF32Create = (
    reinterpret_cast<decltype(layer->scriptArrayF32Create)>(
      ::loadSymbol("puleScriptArrayF32Create", layerName)
    )
  );
  layer->scriptArrayF32Destroy = (
    reinterpret_cast<decltype(layer->scriptArrayF32Destroy)>(
      ::loadSymbol("puleScriptArrayF32Destroy", layerName)
    )
  );
  layer->scriptArrayF32Ptr = (
    reinterpret_cast<decltype(layer->scriptArrayF32Ptr)>(
      ::loadSymbol("puleScriptArrayF32Ptr", layerName)
    )
  );
  layer->scriptArrayF32Append = (
    reinterpret_cast<decltype(layer->scriptArrayF32Append)>(
      ::loadSymbol("puleScriptArrayF32Append", layerName)
    )
  );
  layer->scriptArrayF32Remove = (
    reinterpret_cast<decltype(layer->scriptArrayF32Remove)>(
      ::loadSymbol("puleScriptArrayF32Remove", layerName)
    )
  );
  layer->scriptArrayF32At = (
    reinterpret_cast<decltype(layer->scriptArrayF32At)>(
      ::loadSymbol("puleScriptArrayF32At", layerName)
    )
  );
  layer->scriptArrayF32Length = (
    reinterpret_cast<decltype(layer->scriptArrayF32Length)>(
      ::loadSymbol("puleScriptArrayF32Length", layerName)
    )
  );
  layer->scriptArrayF32ElementByteSize = (
    reinterpret_cast<decltype(layer->scriptArrayF32ElementByteSize)>(
      ::loadSymbol("puleScriptArrayF32ElementByteSize", layerName)
    )
  );
  layer->scriptModuleFileWatch = (
    reinterpret_cast<decltype(layer->scriptModuleFileWatch)>(
      ::loadSymbol("puleScriptModuleFileWatch", layerName)
    )
  );
  // stream
  layer->streamReadByte = (
    reinterpret_cast<decltype(layer->streamReadByte)>(
      ::loadSymbol("puleStreamReadByte", layerName)
    )
  );
  layer->streamPeekByte = (
    reinterpret_cast<decltype(layer->streamPeekByte)>(
      ::loadSymbol("puleStreamPeekByte", layerName)
    )
  );
  layer->streamReadIsDone = (
    reinterpret_cast<decltype(layer->streamReadIsDone)>(
      ::loadSymbol("puleStreamReadIsDone", layerName)
    )
  );
  layer->streamReadDestroy = (
    reinterpret_cast<decltype(layer->streamReadDestroy)>(
      ::loadSymbol("puleStreamReadDestroy", layerName)
    )
  );
  layer->streamReadFromString = (
    reinterpret_cast<decltype(layer->streamReadFromString)>(
      ::loadSymbol("puleStreamReadFromString", layerName)
    )
  );
  layer->streamWriteBytes = (
    reinterpret_cast<decltype(layer->streamWriteBytes)>(
      ::loadSymbol("puleStreamWriteBytes", layerName)
    )
  );
  layer->streamWriteFlush = (
    reinterpret_cast<decltype(layer->streamWriteFlush)>(
      ::loadSymbol("puleStreamWriteFlush", layerName)
    )
  );
  layer->streamWriteDestroy = (
    reinterpret_cast<decltype(layer->streamWriteDestroy)>(
      ::loadSymbol("puleStreamWriteDestroy", layerName)
    )
  );
  layer->streamStdoutWrite = (
    reinterpret_cast<decltype(layer->streamStdoutWrite)>(
      ::loadSymbol("puleStreamStdoutWrite", layerName)
    )
  );
  // string
  layer->stringDefault = (
    reinterpret_cast<decltype(layer->stringDefault)>(
      ::loadSymbol("puleStringDefault", layerName)
    )
  );
  layer->string = (
    reinterpret_cast<decltype(layer->string)>(
      ::loadSymbol("puleString", layerName)
    )
  );
  layer->stringCopy = (
    reinterpret_cast<decltype(layer->stringCopy)>(
      ::loadSymbol("puleStringCopy", layerName)
    )
  );
  layer->stringDestroy = (
    reinterpret_cast<decltype(layer->stringDestroy)>(
      ::loadSymbol("puleStringDestroy", layerName)
    )
  );
  layer->stringAppend = (
    reinterpret_cast<decltype(layer->stringAppend)>(
      ::loadSymbol("puleStringAppend", layerName)
    )
  );
  layer->stringFormat = (
    reinterpret_cast<decltype(layer->stringFormat)>(
      ::loadSymbol("puleStringFormat", layerName)
    )
  );
  layer->stringFormatDefault = (
    reinterpret_cast<decltype(layer->stringFormatDefault)>(
      ::loadSymbol("puleStringFormatDefault", layerName)
    )
  );
  layer->stringView = (
    reinterpret_cast<decltype(layer->stringView)>(
      ::loadSymbol("puleStringView", layerName)
    )
  );
  layer->cStr = (
    reinterpret_cast<decltype(layer->cStr)>(
      ::loadSymbol("puleCStr", layerName)
    )
  );
  layer->stringViewEq = (
    reinterpret_cast<decltype(layer->stringViewEq)>(
      ::loadSymbol("puleStringViewEq", layerName)
    )
  );
  layer->stringViewContains = (
    reinterpret_cast<decltype(layer->stringViewContains)>(
      ::loadSymbol("puleStringViewContains", layerName)
    )
  );
  layer->stringViewEqCStr = (
    reinterpret_cast<decltype(layer->stringViewEqCStr)>(
      ::loadSymbol("puleStringViewEqCStr", layerName)
    )
  );
  layer->stringViewHash = (
    reinterpret_cast<decltype(layer->stringViewHash)>(
      ::loadSymbol("puleStringViewHash", layerName)
    )
  );
  // task-graph
  layer->taskGraphCreate = (
    reinterpret_cast<decltype(layer->taskGraphCreate)>(
      ::loadSymbol("puleTaskGraphCreate", layerName)
    )
  );
  layer->taskGraphDestroy = (
    reinterpret_cast<decltype(layer->taskGraphDestroy)>(
      ::loadSymbol("puleTaskGraphDestroy", layerName)
    )
  );
  layer->taskGraphMerge = (
    reinterpret_cast<decltype(layer->taskGraphMerge)>(
      ::loadSymbol("puleTaskGraphMerge", layerName)
    )
  );
  layer->taskGraphNodeCreate = (
    reinterpret_cast<decltype(layer->taskGraphNodeCreate)>(
      ::loadSymbol("puleTaskGraphNodeCreate", layerName)
    )
  );
  layer->taskGraphNodeRemove = (
    reinterpret_cast<decltype(layer->taskGraphNodeRemove)>(
      ::loadSymbol("puleTaskGraphNodeRemove", layerName)
    )
  );
  layer->taskGraphNodeLabel = (
    reinterpret_cast<decltype(layer->taskGraphNodeLabel)>(
      ::loadSymbol("puleTaskGraphNodeLabel", layerName)
    )
  );
  layer->taskGraphNodeFetch = (
    reinterpret_cast<decltype(layer->taskGraphNodeFetch)>(
      ::loadSymbol("puleTaskGraphNodeFetch", layerName)
    )
  );
  layer->taskGraphNodeAttributeStore = (
    reinterpret_cast<decltype(layer->taskGraphNodeAttributeStore)>(
      ::loadSymbol("puleTaskGraphNodeAttributeStore", layerName)
    )
  );
  layer->taskGraphNodeAttributeStoreU64 = (
    reinterpret_cast<decltype(layer->taskGraphNodeAttributeStoreU64)>(
      ::loadSymbol("puleTaskGraphNodeAttributeStoreU64", layerName)
    )
  );
  layer->taskGraphNodeAttributeFetch = (
    reinterpret_cast<decltype(layer->taskGraphNodeAttributeFetch)>(
      ::loadSymbol("puleTaskGraphNodeAttributeFetch", layerName)
    )
  );
  layer->taskGraphNodeAttributeFetchU64 = (
    reinterpret_cast<decltype(layer->taskGraphNodeAttributeFetchU64)>(
      ::loadSymbol("puleTaskGraphNodeAttributeFetchU64", layerName)
    )
  );
  layer->taskGraphNodeAttributeRemove = (
    reinterpret_cast<decltype(layer->taskGraphNodeAttributeRemove)>(
      ::loadSymbol("puleTaskGraphNodeAttributeRemove", layerName)
    )
  );
  layer->taskGraphNodeRelationSet = (
    reinterpret_cast<decltype(layer->taskGraphNodeRelationSet)>(
      ::loadSymbol("puleTaskGraphNodeRelationSet", layerName)
    )
  );
  layer->taskGraphExecuteInOrder = (
    reinterpret_cast<decltype(layer->taskGraphExecuteInOrder)>(
      ::loadSymbol("puleTaskGraphExecuteInOrder", layerName)
    )
  );
  layer->taskGraphNodeExists = (
    reinterpret_cast<decltype(layer->taskGraphNodeExists)>(
      ::loadSymbol("puleTaskGraphNodeExists", layerName)
    )
  );
  // time
  layer->microsecond = (
    reinterpret_cast<decltype(layer->microsecond)>(
      ::loadSymbol("puleMicrosecond", layerName)
    )
  );
  layer->sleepMicrosecond = (
    reinterpret_cast<decltype(layer->sleepMicrosecond)>(
      ::loadSymbol("puleSleepMicrosecond", layerName)
    )
  );
  // tui
  layer->tuiInitialize = (
    reinterpret_cast<decltype(layer->tuiInitialize)>(
      ::loadSymbol("puleTuiInitialize", layerName)
    )
  );
  layer->tuiDestroy = (
    reinterpret_cast<decltype(layer->tuiDestroy)>(
      ::loadSymbol("puleTuiDestroy", layerName)
    )
  );
  layer->tuiClear = (
    reinterpret_cast<decltype(layer->tuiClear)>(
      ::loadSymbol("puleTuiClear", layerName)
    )
  );
  layer->tuiRefresh = (
    reinterpret_cast<decltype(layer->tuiRefresh)>(
      ::loadSymbol("puleTuiRefresh", layerName)
    )
  );
  layer->tuiRenderString = (
    reinterpret_cast<decltype(layer->tuiRenderString)>(
      ::loadSymbol("puleTuiRenderString", layerName)
    )
  );
  layer->tuiReadInputCharacterBlocking = (
    reinterpret_cast<decltype(layer->tuiReadInputCharacterBlocking)>(
      ::loadSymbol("puleTuiReadInputCharacterBlocking", layerName)
    )
  );
  layer->tuiMoveCursor = (
    reinterpret_cast<decltype(layer->tuiMoveCursor)>(
      ::loadSymbol("puleTuiMoveCursor", layerName)
    )
  );
  layer->tuiWindowDim = (
    reinterpret_cast<decltype(layer->tuiWindowDim)>(
      ::loadSymbol("puleTuiWindowDim", layerName)
    )
  );
  // render-graph
  layer->renderGraphCreate = (
    reinterpret_cast<decltype(layer->renderGraphCreate)>(
      ::loadSymbol("puleRenderGraphCreate", layerName)
    )
  );
  layer->renderGraphDestroy = (
    reinterpret_cast<decltype(layer->renderGraphDestroy)>(
      ::loadSymbol("puleRenderGraphDestroy", layerName)
    )
  );
  layer->renderGraphMerge = (
    reinterpret_cast<decltype(layer->renderGraphMerge)>(
      ::loadSymbol("puleRenderGraphMerge", layerName)
    )
  );
  layer->renderGraphNodeCreate = (
    reinterpret_cast<decltype(layer->renderGraphNodeCreate)>(
      ::loadSymbol("puleRenderGraphNodeCreate", layerName)
    )
  );
  layer->renderGraphNodeRemove = (
    reinterpret_cast<decltype(layer->renderGraphNodeRemove)>(
      ::loadSymbol("puleRenderGraphNodeRemove", layerName)
    )
  );
  layer->renderGraphNodeLabel = (
    reinterpret_cast<decltype(layer->renderGraphNodeLabel)>(
      ::loadSymbol("puleRenderGraphNodeLabel", layerName)
    )
  );
  layer->renderGraphNodeFetch = (
    reinterpret_cast<decltype(layer->renderGraphNodeFetch)>(
      ::loadSymbol("puleRenderGraphNodeFetch", layerName)
    )
  );
  layer->renderGraph_resourceCreate = (
    reinterpret_cast<decltype(layer->renderGraph_resourceCreate)>(
      ::loadSymbol("puleRenderGraph_resourceCreate", layerName)
    )
  );
  layer->renderGraph_resource = (
    reinterpret_cast<decltype(layer->renderGraph_resource)>(
      ::loadSymbol("puleRenderGraph_resource", layerName)
    )
  );
  layer->renderGraph_resourceRemove = (
    reinterpret_cast<decltype(layer->renderGraph_resourceRemove)>(
      ::loadSymbol("puleRenderGraph_resourceRemove", layerName)
    )
  );
  layer->renderGraph_node_resourceAssign = (
    reinterpret_cast<decltype(layer->renderGraph_node_resourceAssign)>(
      ::loadSymbol("puleRenderGraph_node_resourceAssign", layerName)
    )
  );
  layer->renderGraph_commandList = (
    reinterpret_cast<decltype(layer->renderGraph_commandList)>(
      ::loadSymbol("puleRenderGraph_commandList", layerName)
    )
  );
  layer->renderGraph_commandListRecorder = (
    reinterpret_cast<decltype(layer->renderGraph_commandListRecorder)>(
      ::loadSymbol("puleRenderGraph_commandListRecorder", layerName)
    )
  );
  layer->renderGraphNodeRelationSet = (
    reinterpret_cast<decltype(layer->renderGraphNodeRelationSet)>(
      ::loadSymbol("puleRenderGraphNodeRelationSet", layerName)
    )
  );
  layer->renderGraphFrameStart = (
    reinterpret_cast<decltype(layer->renderGraphFrameStart)>(
      ::loadSymbol("puleRenderGraphFrameStart", layerName)
    )
  );
  layer->renderGraphFrameSubmit = (
    reinterpret_cast<decltype(layer->renderGraphFrameSubmit)>(
      ::loadSymbol("puleRenderGraphFrameSubmit", layerName)
    )
  );
  layer->renderGraphExecuteInOrder = (
    reinterpret_cast<decltype(layer->renderGraphExecuteInOrder)>(
      ::loadSymbol("puleRenderGraphExecuteInOrder", layerName)
    )
  );
  layer->renderGraphNodeExists = (
    reinterpret_cast<decltype(layer->renderGraphNodeExists)>(
      ::loadSymbol("puleRenderGraphNodeExists", layerName)
    )
  );
  // gpu
  layer->gpuShaderModuleCreate = (
    reinterpret_cast<decltype(layer->gpuShaderModuleCreate)>(
      ::loadSymbol("puleGpuShaderModuleCreate", layerName)
    )
  );
  layer->gpuShaderModuleDestroy = (
    reinterpret_cast<decltype(layer->gpuShaderModuleDestroy)>(
      ::loadSymbol("puleGpuShaderModuleDestroy", layerName)
    )
  );
  layer->gpuPipelineDescriptorSetLayout = (
    reinterpret_cast<decltype(layer->gpuPipelineDescriptorSetLayout)>(
      ::loadSymbol("puleGpuPipelineDescriptorSetLayout", layerName)
    )
  );
  layer->gpuPipelineCreate = (
    reinterpret_cast<decltype(layer->gpuPipelineCreate)>(
      ::loadSymbol("puleGpuPipelineCreate", layerName)
    )
  );
  layer->gpuPipelineDestroy = (
    reinterpret_cast<decltype(layer->gpuPipelineDestroy)>(
      ::loadSymbol("puleGpuPipelineDestroy", layerName)
    )
  );
  layer->gpuSamplerCreate = (
    reinterpret_cast<decltype(layer->gpuSamplerCreate)>(
      ::loadSymbol("puleGpuSamplerCreate", layerName)
    )
  );
  layer->gpuSamplerDestroy = (
    reinterpret_cast<decltype(layer->gpuSamplerDestroy)>(
      ::loadSymbol("puleGpuSamplerDestroy", layerName)
    )
  );
  layer->gpuImageLabel = (
    reinterpret_cast<decltype(layer->gpuImageLabel)>(
      ::loadSymbol("puleGpuImageLabel", layerName)
    )
  );
  layer->gpuImageDestroy = (
    reinterpret_cast<decltype(layer->gpuImageDestroy)>(
      ::loadSymbol("puleGpuImageDestroy", layerName)
    )
  );
  layer->gpuImageCreate = (
    reinterpret_cast<decltype(layer->gpuImageCreate)>(
      ::loadSymbol("puleGpuImageCreate", layerName)
    )
  );
  layer->gpuFramebufferCreateInfo = (
    reinterpret_cast<decltype(layer->gpuFramebufferCreateInfo)>(
      ::loadSymbol("puleGpuFramebufferCreateInfo", layerName)
    )
  );
  layer->gpuFramebufferCreate = (
    reinterpret_cast<decltype(layer->gpuFramebufferCreate)>(
      ::loadSymbol("puleGpuFramebufferCreate", layerName)
    )
  );
  layer->gpuFramebufferDestroy = (
    reinterpret_cast<decltype(layer->gpuFramebufferDestroy)>(
      ::loadSymbol("puleGpuFramebufferDestroy", layerName)
    )
  );
  layer->gpuImageLayoutLabel = (
    reinterpret_cast<decltype(layer->gpuImageLayoutLabel)>(
      ::loadSymbol("puleGpuImageLayoutLabel", layerName)
    )
  );
  layer->gpuActionToString = (
    reinterpret_cast<decltype(layer->gpuActionToString)>(
      ::loadSymbol("puleGpuActionToString", layerName)
    )
  );
  layer->gpuCommandListCreate = (
    reinterpret_cast<decltype(layer->gpuCommandListCreate)>(
      ::loadSymbol("puleGpuCommandListCreate", layerName)
    )
  );
  layer->gpuCommandListDestroy = (
    reinterpret_cast<decltype(layer->gpuCommandListDestroy)>(
      ::loadSymbol("puleGpuCommandListDestroy", layerName)
    )
  );
  layer->gpuCommandListName = (
    reinterpret_cast<decltype(layer->gpuCommandListName)>(
      ::loadSymbol("puleGpuCommandListName", layerName)
    )
  );
  layer->gpuCommandListRecorder = (
    reinterpret_cast<decltype(layer->gpuCommandListRecorder)>(
      ::loadSymbol("puleGpuCommandListRecorder", layerName)
    )
  );
  layer->gpuCommandListRecorderFinish = (
    reinterpret_cast<decltype(layer->gpuCommandListRecorderFinish)>(
      ::loadSymbol("puleGpuCommandListRecorderFinish", layerName)
    )
  );
  layer->gpuCommandListAppendAction = (
    reinterpret_cast<decltype(layer->gpuCommandListAppendAction)>(
      ::loadSymbol("puleGpuCommandListAppendAction", layerName)
    )
  );
  layer->gpuCommandListSubmit = (
    reinterpret_cast<decltype(layer->gpuCommandListSubmit)>(
      ::loadSymbol("puleGpuCommandListSubmit", layerName)
    )
  );
  layer->gpuCommandListSubmitAndPresent = (
    reinterpret_cast<decltype(layer->gpuCommandListSubmitAndPresent)>(
      ::loadSymbol("puleGpuCommandListSubmitAndPresent", layerName)
    )
  );
  layer->gpuCommandListDump = (
    reinterpret_cast<decltype(layer->gpuCommandListDump)>(
      ::loadSymbol("puleGpuCommandListDump", layerName)
    )
  );
  layer->gpuCommandListChainCreate = (
    reinterpret_cast<decltype(layer->gpuCommandListChainCreate)>(
      ::loadSymbol("puleGpuCommandListChainCreate", layerName)
    )
  );
  layer->gpuCommandListChainDestroy = (
    reinterpret_cast<decltype(layer->gpuCommandListChainDestroy)>(
      ::loadSymbol("puleGpuCommandListChainDestroy", layerName)
    )
  );
  layer->gpuCommandListChainCurrent = (
    reinterpret_cast<decltype(layer->gpuCommandListChainCurrent)>(
      ::loadSymbol("puleGpuCommandListChainCurrent", layerName)
    )
  );
  layer->gpuCommandListChainCurrentFence = (
    reinterpret_cast<decltype(layer->gpuCommandListChainCurrentFence)>(
      ::loadSymbol("puleGpuCommandListChainCurrentFence", layerName)
    )
  );
  layer->gpuResourceBarrierStageLabel = (
    reinterpret_cast<decltype(layer->gpuResourceBarrierStageLabel)>(
      ::loadSymbol("puleGpuResourceBarrierStageLabel", layerName)
    )
  );
  layer->gpuResourceAccessLabel = (
    reinterpret_cast<decltype(layer->gpuResourceAccessLabel)>(
      ::loadSymbol("puleGpuResourceAccessLabel", layerName)
    )
  );
  layer->gpuBufferCreate = (
    reinterpret_cast<decltype(layer->gpuBufferCreate)>(
      ::loadSymbol("puleGpuBufferCreate", layerName)
    )
  );
  layer->gpuBufferDestroy = (
    reinterpret_cast<decltype(layer->gpuBufferDestroy)>(
      ::loadSymbol("puleGpuBufferDestroy", layerName)
    )
  );
  layer->gpuBufferMap = (
    reinterpret_cast<decltype(layer->gpuBufferMap)>(
      ::loadSymbol("puleGpuBufferMap", layerName)
    )
  );
  layer->gpuBufferMappedFlush = (
    reinterpret_cast<decltype(layer->gpuBufferMappedFlush)>(
      ::loadSymbol("puleGpuBufferMappedFlush", layerName)
    )
  );
  layer->gpuBufferUnmap = (
    reinterpret_cast<decltype(layer->gpuBufferUnmap)>(
      ::loadSymbol("puleGpuBufferUnmap", layerName)
    )
  );
  layer->gpuInitialize = (
    reinterpret_cast<decltype(layer->gpuInitialize)>(
      ::loadSymbol("puleGpuInitialize", layerName)
    )
  );
  layer->gpuShutdown = (
    reinterpret_cast<decltype(layer->gpuShutdown)>(
      ::loadSymbol("puleGpuShutdown", layerName)
    )
  );
  layer->gpuDebugPrint = (
    reinterpret_cast<decltype(layer->gpuDebugPrint)>(
      ::loadSymbol("puleGpuDebugPrint", layerName)
    )
  );
  layer->gpuBufferUsageLabel = (
    reinterpret_cast<decltype(layer->gpuBufferUsageLabel)>(
      ::loadSymbol("puleGpuBufferUsageLabel", layerName)
    )
  );
  layer->gpuSemaphoreCreate = (
    reinterpret_cast<decltype(layer->gpuSemaphoreCreate)>(
      ::loadSymbol("puleGpuSemaphoreCreate", layerName)
    )
  );
  layer->gpuSemaphoreDestroy = (
    reinterpret_cast<decltype(layer->gpuSemaphoreDestroy)>(
      ::loadSymbol("puleGpuSemaphoreDestroy", layerName)
    )
  );
  layer->gpuFenceCreate = (
    reinterpret_cast<decltype(layer->gpuFenceCreate)>(
      ::loadSymbol("puleGpuFenceCreate", layerName)
    )
  );
  layer->gpuFenceDestroy = (
    reinterpret_cast<decltype(layer->gpuFenceDestroy)>(
      ::loadSymbol("puleGpuFenceDestroy", layerName)
    )
  );
  layer->gpuFenceWaitSignal = (
    reinterpret_cast<decltype(layer->gpuFenceWaitSignal)>(
      ::loadSymbol("puleGpuFenceWaitSignal", layerName)
    )
  );
  layer->gpuFenceReset = (
    reinterpret_cast<decltype(layer->gpuFenceReset)>(
      ::loadSymbol("puleGpuFenceReset", layerName)
    )
  );
  layer->gpuFrameStart = (
    reinterpret_cast<decltype(layer->gpuFrameStart)>(
      ::loadSymbol("puleGpuFrameStart", layerName)
    )
  );
  layer->gpuFrameEnd = (
    reinterpret_cast<decltype(layer->gpuFrameEnd)>(
      ::loadSymbol("puleGpuFrameEnd", layerName)
    )
  );
  layer->gpuImageChain_create = (
    reinterpret_cast<decltype(layer->gpuImageChain_create)>(
      ::loadSymbol("puleGpuImageChain_create", layerName)
    )
  );
  layer->gpuImageChain_destroy = (
    reinterpret_cast<decltype(layer->gpuImageChain_destroy)>(
      ::loadSymbol("puleGpuImageChain_destroy", layerName)
    )
  );
  layer->gpuImageChain_current = (
    reinterpret_cast<decltype(layer->gpuImageChain_current)>(
      ::loadSymbol("puleGpuImageChain_current", layerName)
    )
  );
  layer->gpuImageReference_createImageChain = (
    reinterpret_cast<decltype(layer->gpuImageReference_createImageChain)>(
      ::loadSymbol("puleGpuImageReference_createImageChain", layerName)
    )
  );
  layer->gpuImageReference_createImage = (
    reinterpret_cast<decltype(layer->gpuImageReference_createImage)>(
      ::loadSymbol("puleGpuImageReference_createImage", layerName)
    )
  );
  layer->gpuImageReference_updateImageChain = (
    reinterpret_cast<decltype(layer->gpuImageReference_updateImageChain)>(
      ::loadSymbol("puleGpuImageReference_updateImageChain", layerName)
    )
  );
  layer->gpuImageReference_updateImage = (
    reinterpret_cast<decltype(layer->gpuImageReference_updateImage)>(
      ::loadSymbol("puleGpuImageReference_updateImage", layerName)
    )
  );
  layer->gpuImageReference_destroy = (
    reinterpret_cast<decltype(layer->gpuImageReference_destroy)>(
      ::loadSymbol("puleGpuImageReference_destroy", layerName)
    )
  );
  layer->gpuImageReference_image = (
    reinterpret_cast<decltype(layer->gpuImageReference_image)>(
      ::loadSymbol("puleGpuImageReference_image", layerName)
    )
  );
  layer->gpuWindowSwapchainImage = (
    reinterpret_cast<decltype(layer->gpuWindowSwapchainImage)>(
      ::loadSymbol("puleGpuWindowSwapchainImage", layerName)
    )
  );
  layer->gpuWindowSwapchainImageReference = (
    reinterpret_cast<decltype(layer->gpuWindowSwapchainImageReference)>(
      ::loadSymbol("puleGpuWindowSwapchainImageReference", layerName)
    )
  );
  // physx
  layer->physxRaycastShape = (
    reinterpret_cast<decltype(layer->physxRaycastShape)>(
      ::loadSymbol("pulePhysxRaycastShape", layerName)
    )
  );
  layer->physxRaycastMesh = (
    reinterpret_cast<decltype(layer->physxRaycastMesh)>(
      ::loadSymbol("pulePhysxRaycastMesh", layerName)
    )
  );
  layer->physxShapeOverlapShape = (
    reinterpret_cast<decltype(layer->physxShapeOverlapShape)>(
      ::loadSymbol("pulePhysxShapeOverlapShape", layerName)
    )
  );
  layer->physxShapeOverlapMesh = (
    reinterpret_cast<decltype(layer->physxShapeOverlapMesh)>(
      ::loadSymbol("pulePhysxShapeOverlapMesh", layerName)
    )
  );
  layer->physx2DWorldCreate = (
    reinterpret_cast<decltype(layer->physx2DWorldCreate)>(
      ::loadSymbol("pulePhysx2DWorldCreate", layerName)
    )
  );
  layer->physx2DWorldDestroy = (
    reinterpret_cast<decltype(layer->physx2DWorldDestroy)>(
      ::loadSymbol("pulePhysx2DWorldDestroy", layerName)
    )
  );
  layer->physx2DWorldGravitySet = (
    reinterpret_cast<decltype(layer->physx2DWorldGravitySet)>(
      ::loadSymbol("pulePhysx2DWorldGravitySet", layerName)
    )
  );
  layer->physx2DWorldStep = (
    reinterpret_cast<decltype(layer->physx2DWorldStep)>(
      ::loadSymbol("pulePhysx2DWorldStep", layerName)
    )
  );
  layer->physx2DBodyCreate = (
    reinterpret_cast<decltype(layer->physx2DBodyCreate)>(
      ::loadSymbol("pulePhysx2DBodyCreate", layerName)
    )
  );
  layer->physx2DBodyDestroy = (
    reinterpret_cast<decltype(layer->physx2DBodyDestroy)>(
      ::loadSymbol("pulePhysx2DBodyDestroy", layerName)
    )
  );
  layer->physx2DBodyPosition = (
    reinterpret_cast<decltype(layer->physx2DBodyPosition)>(
      ::loadSymbol("pulePhysx2DBodyPosition", layerName)
    )
  );
  layer->physx2DBodyAngle = (
    reinterpret_cast<decltype(layer->physx2DBodyAngle)>(
      ::loadSymbol("pulePhysx2DBodyAngle", layerName)
    )
  );
  layer->physx2DBodyAttachShape = (
    reinterpret_cast<decltype(layer->physx2DBodyAttachShape)>(
      ::loadSymbol("pulePhysx2DBodyAttachShape", layerName)
    )
  );
  layer->physx2DShapeCreateConvexPolygonAsBox = (
    reinterpret_cast<decltype(layer->physx2DShapeCreateConvexPolygonAsBox)>(
      ::loadSymbol("pulePhysx2DShapeCreateConvexPolygonAsBox", layerName)
    )
  );
