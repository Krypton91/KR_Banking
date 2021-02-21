class RobAtmSpawns
{
    protected vector Position;
    protected vector Direction;
    string RobMessage;

    void RobAtmSpawns(vector atmPos, vector atmDirection, string robmsg)
    {
        Position = atmPos;
        Direction = atmDirection;
        RobMessage = robmsg;
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