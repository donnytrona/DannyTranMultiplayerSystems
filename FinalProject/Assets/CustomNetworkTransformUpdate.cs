using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;

public class TransformMessage : MessageBase
{
    public uint netID;
    public Vector3 position;
    public Quaternion rotation;
}

public class CustomNetworkTransformUpdate : NetworkBehaviour
{

    public float m_updateTransformTime = 0.25f;
    private float m_currentUpdateTransformTime = 0.0f;

    NetworkClient m_client;

    // Use this for initialization
    void Start () {
        m_client = isServer ? null : NetworkManager.singleton.client;

        //only replicas need to listen for transform updates
        if (!isLocalPlayer)
        {
            Debug.Log("Register Handler Netid: " + this.netId.Value);
            if (isServer)
            {
                NetworkServer.RegisterHandler(CustomMsgType.Transform, OnTransformReceived);
            }
            else
            {
                m_client.RegisterHandler(CustomMsgType.Transform, OnTransformReceived);
            }
        }
    }
	
	// Update is called once per frame
	void Update () {
        m_currentUpdateTransformTime += Time.deltaTime;
        if (m_currentUpdateTransformTime > m_updateTransformTime)
        {
            //send msg
            TransformMessage msg = new TransformMessage();
            msg.position = this.transform.position;
            msg.rotation = this.transform.rotation;
            msg.netID = this.netId.Value;

            if (isServer)
            {
                NetworkServer.SendToAll(CustomMsgType.Transform, msg);
            }
            else
            {
                m_client.Send(CustomMsgType.Transform, msg);
            }
            m_currentUpdateTransformTime = 0.0f;
        }
    }

    public void OnTransformReceived(NetworkMessage netMsg)
    {
        TransformMessage msg = netMsg.ReadMessage<TransformMessage>();
        //Debug.Log("OnTransformReceived: IsLocalPlayer:" + isLocalPlayer);
        //Debug.Log("NetID: " + msg.netID);
        //Debug.Log("MSGPOS: " + msg.position);
        //Debug.Log("OURNETID: " + this.netId.Value);


        //we are the replica of a an object on another machine
        //that object sent out a msg to update our position
        if (msg.netID == this.netId.Value)
        {
            this.transform.position = msg.position;
            this.transform.rotation = msg.rotation;
        }
    }
}
