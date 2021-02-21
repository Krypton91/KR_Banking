class ATMPosition
{
    string ATMSName;
    protected vector Position;
    protected vector Direction;

    void ATMPosition(string atmname, vector atmPos, vector atmDirection)
    {
        ATMSName = atmname;
        Position = atmPos;
        Direction = atmDirection;
    }

    vector GetATMPosition()
    {
        return Position;
    }

    vector GetATMDirectory()
    {
        return Direction;
    }
};