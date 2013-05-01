
class ServerNotFoundError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg


class PeerRegistrationError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg

class UserGetInfoError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg

class AvatarNotFoundError(Exception):
    def __init__(self, msg):
        self.msg = msg
    def __str__(self):
        return self.msg
