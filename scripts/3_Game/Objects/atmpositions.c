class ATMPosition
{
    string ATMSName;
    protected vector Position;
    protected vector Direction;
    protected bool   CanBeRobbed;
    protected string RobMessage;

    void ATMPosition(string atmname, vector atmPos, vector atmDirection, bool CanRob = false, string robmessage = "")
    {
        ATMSName = atmname;
        Position = atmPos;
        Direction = atmDirection;
        CanBeRobbed = CanRob;
        RobMessage = robmessage;
    }

    void SetCanRobbed(bool rob)
    {
        CanBeRobbed = rob;
    }

    vector GetATMPosition()
    {
        return Position;
    }

    vector GetATMDirectory()
    {
        return Direction;
    }

    bool GetCanRobbed()
    {
        return CanBeRobbed;
    }

    string GetRobMessage()
    {
        return RobMessage;
    }
};