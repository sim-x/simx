

#### Define Messages #### (Optional)

class HelloMessage:
    def __init__(self,neighbors):
        self.neighbors = neighbors

class ReplyMessage:
    def __init__(self,data=None):
        self.data = data
