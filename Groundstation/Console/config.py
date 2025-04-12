"""config for the Console and Serial handler."""

# Serial handler
## Virtual Com Port Settings
COM_PORT = "/dev/ttyACM0" #"automatic"
STANDART_BAUD = 115200

## Saving Formats
PRINT_RECIEVED_PACKAGES = True  # Toggle here to print recieved packages diectly to the console

SAVE_IN_BUFFER_FORMATED = False  # Toggle here to save packets in the list BUFFER_FORMATED for z.B: a matlab plot

SAVE_AS_CSV = False  # Toggle here the generatrion of a CSV
FILEPATH_CSV = "data.csv"

SAVE_AS_JSON = False  # Toggle here the generatrion of a JONSON
FILEPATH_JSON = "data.json"

## data formats
PACKET_FORMAT = "I9f"

PACKET_NAMES = [
    "timestamp",
    "v_gate_set",
    "v_optik0_set",
    "v_optik1_set",
    "v_gate_measure",
    "v_optik0_measure",
    "v_optik1_measure",
    "i_emittor_measure",
    "v_hv_mesure",
    "i_anode_measure",
]

PACKAGE_SPECIFIER = 3  # b'x03'   # this vlaue gets send 3 times before a package to difference a package from a debug message, wich just gets send
END_OF_TRAMISSION_MARKER = (
    b"\x04"  # EOT character. gets send after a package or a debug message
)

TIMEOUT_RECONNECT = 0.1  # in s
DELAY_ERROR = 0.1  # in s

# Console
DELAY_CONSOLE_LOOP = 0.1  # in s. slows console loop to save performance
DELAY_ERROR = 2  # in s. delay to prevent errors to be spammed
