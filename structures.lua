local structures = {}

structures.DRIVER_TYPE = {
    NULL = 0,
    SOFTWARE = 1,
    BURNINGSVIDEO = 2,
    DIRECT3D8 = 3,
    DIRECT3D9 = 4,
    OPENGL = 5,
    COUNT = 6
}

structures.ANTI_ALIASING_MODE = {
    OFF = 0,
    SIMPLE = 1,
    QUALITY = 2,
    LINE_SMOOTH = 3,
    POINT_SMOOTH = 4,
    FULL_BASIC = 5,
    ALPHA_TO_COVERAGE = 6
}

structures.TRANSFORM_TYPE = {
    POSITION = 0,
    ROTATION = 1,
    SCALE = 2
}

structures.BLEND_OPERATION = {
    NONE = 0,
    ADD = 1,
    SUBTRACT = 2,
    REVSUBTRACT = 3,
    MIN = 4,
    MAX = 5,
    MIN_FACTOR = 6,
    MAX_FACTOR = 7,
    MIN_ALPHA = 8,
    MAX_ALPHA = 9
}

structures.COLOR_MATERIAL = {
    NONE = 0,
    DIFFUSE = 1,
    AMBIENT = 2,
    EMISSIVE = 3,
    SPECULAR = 4,
    DIFFUSE_AND_AMBIENT = 5
}

structures.COLOR_PLANE = {
    NONE = 0,
    ALPHA = 1,
    RED = 2,
    GREEN = 3,
    BLUE = 4,
    RGB = 5,
    ALL = 6
}

structures.MESSAGE_ICON = {
    OK = 0,
    MESSAGE = 1,
    WARNING = 2,
    INFORMATION = 3
}

structures.PARTICLE_EMITTER_TYPE = {
    POINT = 0,
    CUBE = 1,
    SPHERE = 2,
    RING = 3
}

structures.PARTICLE_AFFECTOR_TYPE = {
    ATTRACT = 0,
    FADE_OUT = 1,
    GRAVITY = 2,
    ROTATION = 3,
    SCALE = 4
}

structures.TRAIL_ALIGNMENT_TYPE = {
    CAMERA_X_AXIS = 0,
    CAMERA_Y_AXIS = 1,
    GLOBAL_X_AXIS = 2,
    GLOBAL_Y_AXIS = 3,
    GLOBAL_Z_AXIS = 4,
    PARENT_X_AXIS = 5,
    PARENT_Y_AXIS = 6,
    PARENT_Z_AXIS = 7
}

structures.LEGACY_LIGHT_TYPE = {
    POINT = 0,
    SPOT = 1,
    DIRECTIONAL = 2
}

structures.LIGHT_MANAGEMENT_MODE = {
    DISTANCE_TO_CAMERA = 0,
    NEAREST_TO_OBJECTS = 1,
    ZONE = 2
}

structures.COMPARISON_FUNC = {
    NEVER = 0,
    LESSEQUAL = 1,
    EQUAL = 2,
    NOTEQUAL = 3,
    GREATEREQUAL = 4,
    GREATER = 5,
    ALWAYS = 6
}

structures.FOG_TYPE = {
    EXP = 0,
    LINEAR = 1,
    EXP2 = 2
}

structures.ALIGNMENT_TYPE = {
    LEFT = 0,
    RIGHT = 1,
    CENTER = 2
}

structures.ANCHOR_TYPE = {
    TOP_LEFT = 0,
    TOP_CENTER = 1,
    TOP_RIGHT = 2,
    CENTER_LEFT = 3,
    CENTER = 4,
    CENTER_RIGHT = 5,
    BOTTOM_LEFT = 6,
    BOTTOM_CENTER = 7,
    BOTTOM_RIGHT = 8
}

structures.MATERIAL_TYPE = {
    SOLID = 0,
    SOLID_2_LAYER = 1,
    LIGHTMAP = 2,
    LIGHTMAP_ADD = 3,
    LIGHTMAP_M2 = 4,
    LIGHTMAP_M4 = 5,
    LIGHTMAP_LIGHTING = 6,
    LIGHTMAP_LIGHTING_M2 = 7,
    LIGHTMAP_LIGHTING_M4 = 8,
    DETAIL_MAP = 9,
    SPHERE_MAP = 10,
    REFLECTION_2_LAYER = 11,
    TRANSPARENT_ADD_COLOR = 12,
    TRANSPARENT_ALPHA_CHANNEL = 13,
    TRANSPARENT_ALPHA_CHANNEL_REF = 14,
    TRANSPARENT_VERTEX_ALPHA = 15,
    TRANSPARENT_REFLECTION_2_LAYER = 16,
    NORMAL_MAP_SOLID = 17,
    NORMAL_MAP_TRANSPARENT_ADD_COLOR = 18,
    NORMAL_MAP_TRANSPARENT_VERTEX_ALPHA = 19,
    PARALLAX_MAP_SOLID = 20,
    PARALLAX_MAP_TRANSPARENT_ADD_COLOR = 21,
    PARALLAX_MAP_TRANSPARENT_VERTEX_ALPHA = 22,
    ONETEXTURE_BLEND = 23,
    FORCE_32BIT = 24
}

structures.MATERIAL_FLAG = {
    WIREFRAME = 0x1,
    POINTCLOUD = 0x2,
    GOURAUD_SHADING = 0x4,
    LIGHTING = 0x8,
    ZBUFFER = 0x10,
    ZWRITE_ENABLE = 0x20,
    BACK_FACE_CULLING = 0x40,
    FRONT_FACE_CULLING = 0x80,
    BILINEAR_FILTER = 0x100,
    TRILINEAR_FILTER = 0x200,
    ANISOTROPIC_FILTER = 0x400,
    FOG_ENABLE = 0x800,
    NORMALIZE_NORMALS = 0x1000,
    TEXTURE_WRAP = 0x2000,
    ANTI_ALIASING = 0x4000,
    COLOR_MASK = 0x8000,
    COLOR_MATERIAL = 0x10000,
    USE_MIP_MAPS = 0x20000,
    BLEND_OPERATION = 0x40000,
    POLYGON_OFFSET = 0x80000
}

structures.GUI_ALIGNMENT = {
    UPPER_LEFT = 0,
    LOWER_RIGHT = 1,
    CENTER = 2,
    SCALE = 3
}

structures.POLYGON_OFFSET = {
    BACK = 0,
    FRONT = 1
}

structures.HARDWARE_MAPPING_HINT = {
    NEVER = 0,
    STATIC = 1,
    DYNAMIC = 2,
    STREAM = 3
}

structures.SHADOW_FILTER_SAMPLING = {
    NONE = 0,
    SAMPLING_4 = 1,
    SAMPLING_8 = 2,
    SAMPLING_12 = 3,
    SAMPLING_16 = 4,
    COUNT = 5 -- ?
}

structures.SHADOW_RESOLUTION = {
    RES_256 = 0,
    RES_512 = 1,
    RES_1024 = 2,
    RES_2048 = 3,
    RES_4096 = 4
}

structures.SHADOW_MODE = {
    RECEIVE = 0,
    CAST = 1,
    BOTH = 2,
    COUNT = 3,
    NONE = 4
}

structures.RENDER_TARGET = {
    FRAME_BUFFER = 0,
    RENDER_TEXTURE = 1,
    MULTI_RENDER_TEXTURES = 2,
    STEREO_LEFT_BUFFER = 3,
    STEREO_RIGHT_BUFFER = 4,
    STEREO_BOTH_BUFFERS = 5,
    AUX_BUFFER0 = 6,
    AUX_BUFFER1 = 7,
    AUX_BUFFER2 = 8,
    AUX_BUFFER3 = 9,
    AUX_BUFFER4 = 10
}

structures.TEXTURE_CLAMP = {
    REPEAT = 0,
    CLAMP = 1,
    CLAMP_TO_EDGE = 2,
    CLAMP_TO_BORDER = 3,
    MIRROR = 4,
    MIRROR_CLAMP = 5,
    MIRROR_CLAMP_TO_EDGE = 6,
    MIRROR_CLAMP_TO_BORDER = 7
}

structures.TEXTURE_CREATION_FLAG = {
    ALWAYS_16_BIT = 0,
    ALWAYS_32_BIT = 1,
    OPTIMIZED_FOR_QUALITY = 2,
    OPTIMIZED_FOR_SPEED = 3,
    CREATE_MIP_MAPS = 4,
    NO_ALPHA_CHANNEL = 5,
    ALLOW_NON_POWER_2 = 6
}

structures.MOUSE_CODE = {
    LEFT = 0,
    RIGHT = 1,
    MIDDLE = 2
}

structures.KEY_CODE = {
    LBUTTON = 0,
    RBUTTON = 1,
    CANCEL = 2,
    MBUTTON = 3,
    XBUTTON1 = 4,
    XBUTTON2 = 5,
    BACK = 8,
    TAB = 9,
    CLEAR = 12,
    RETURN = 13,
    SHIFT = 16,
    CONTROL = 17,
    MENU = 18,
    PAUSE = 19,
    CAPITAL = 20,
    KANA = 21,
    HANGUEL = 21,
    HANGUL = 21,
    JUNJA = 23,
    FINAL = 24,
    HANJA = 25,
    KANJI = 25,
    ESCAPE = 27,
    CONVERT = 28,
    NONCONVERT = 29,
    ACCEPT = 30,
    MODECHANGE = 31,
    SPACE = 32,
    PRIOR = 33,
    NEXT = 34,
    END = 35,
    HOME = 36,
    LEFT = 37,
    UP = 38,
    RIGHT = 39,
    DOWN = 40,
    SELECT = 41,
    PRINT = 42,
    EXECUT = 43,
    SNAPSHOT = 44,
    INSERT = 45,
    DELETE = 46,
    HELP = 47,
    KEY_0 = 48,
    KEY_1 = 49,
    KEY_2 = 50,
    KEY_3 = 51,
    KEY_4 = 52,
    KEY_5 = 53,
    KEY_6 = 54,
    KEY_7 = 55,
    KEY_8 = 56,
    KEY_9 = 57,
    A = 65,
    B = 66,
    C = 67,
    D = 68,
    E = 69,
    F = 70,
    G = 71,
    H = 72,
    I = 73,
    J = 74,
    K = 75,
    L = 76,
    M = 77,
    N = 78,
    O = 79,
    P = 80,
    Q = 81,
    R = 82,
    S = 83,
    T = 84,
    U = 85,
    V = 86,
    W = 87,
    X = 88,
    Y = 89,
    Z = 90,
    LWIN = 91,
    RWIN = 92,
    APPS = 93,
    SLEEP = 95,
    NUMPAD0 = 96,
    NUMPAD1 = 97,
    NUMPAD2 = 98,
    NUMPAD3 = 99,
    NUMPAD4 = 100,
    NUMPAD5 = 101,
    NUMPAD6 = 102,
    NUMPAD7 = 103,
    NUMPAD8 = 104,
    NUMPAD9 = 105,
    MULTIPLY = 106,
    ADD = 107,
    SEPARATOR = 108,
    SUBTRACT = 109,
    DECIMAL = 110,
    DIVIDE = 111,
    F1 = 112,
    F2 = 113,
    F3 = 114,
    F4 = 115,
    F5 = 116,
    F6 = 117,
    F7 = 118,
    F8 = 119,
    F9 = 120,
    F10 = 121,
    F11 = 122,
    F12 = 123,
    NUMLOCK = 144,
    SCROLL = 145,
    LSHIFT = 160,
    RSHIFT = 161,
    LCONTROL = 162,
    RCONTROL = 163,
    LMENU = 164,
    RMENU = 165,
    OEM_1 = 186,
    PLUS = 187,
    COMMA = 188,
    MINUS = 189,
    PERIOD = 190,
    OEM_2 = 191,
    OEM_3 = 192,
    OEM_4 = 219,
    OEM_5 = 220,
    OEM_6 = 221,
    OEM_7 = 222,
    OEM_8 = 223,
    OEM_AX = 225,
    OEM_102 = 226,
    ATTN = 246,
    CRSEL = 247,
    EXSEL = 248,
    EREOF = 249,
    PLAY = 250,
    ZOOM = 251,
    PA1 = 252,
    OEM_CLEAR = 254,
    CODES_COUNT = 255
}

structures.PEER_STATE = {
    DISCONNECTED = 0,
    CONNECTING = 1,
    ACKNOWLEDGING_CONNECT = 2,
    CONNECTION_PENDING = 3,
    CONNECTION_SUCCEEDED = 4,
    CONNECTED = 5,
    DISCONNECT_LATER = 6,
    DISCONNECTING = 7,
    ACKNOWLEDGING_DISCONNECT = 8,
    ZOMBIE = 9
}

structures.SOUND_EFFECT = {
    DISTORTION = 0,
    ECHO = 1,
    REVERB = 2
}

structures.DATA_TYPE = {
    BYTE = 0,
    SHORT = 1,
    INTEGER = 2,
    FLOAT = 3,
    STRING = 4,
    FILE = 5
}

return structures