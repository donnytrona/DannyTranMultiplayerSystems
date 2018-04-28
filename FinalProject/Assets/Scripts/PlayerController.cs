using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

public class PlayerController : NetworkBehaviour
{
    public GameObject bulletPrefab;
    public Transform bulletSpawn;

    [SyncVar]
    public int id;

    [SyncVar]
    public bool isPoweredUp = false;

    [SyncVar]
    public bool hasFlag = false;

    [SyncVar]
    public bool frozen = false;

    private float powerUpTime = 5.0f;

    public float m_linearSpeed;

    NetworkIdentity m_Identity;

    public Text playerNumber;

    void Start()
    {
        m_linearSpeed = 0.5f;

        if (!isServer)
        {
            return;
        }

        m_Identity = GetComponent<NetworkIdentity>();
        id = int.Parse(m_Identity.netId.ToString()) - 1;
    }

    void Update()
    {
        if (!isLocalPlayer)
        {
            return;
        }

        playerNumber = GameObject.FindGameObjectWithTag("Player Number").GetComponent<Text>();
        playerNumber.text = "Player Number - " + id;

        if(isPoweredUp && powerUpTime >= 0)
        {
            powerUpTime -= Time.deltaTime;
        }
        else
        {
            isPoweredUp = false;
            powerUpTime = 5.0f;
            m_linearSpeed = 1.0f;
        }

        var x = 0.0f;
        var z = 0.0f;

        if (!frozen)
        {
            x = Input.GetAxis("Horizontal") * Time.deltaTime * 150.0f;
            z = Input.GetAxis("Vertical") * Time.deltaTime * 3.0f * m_linearSpeed;
        }

        transform.Rotate(0, x, 0);
        transform.Translate(0, 0, z);

        if (Input.GetKeyDown(KeyCode.Space))
        {
            CmdFire();
        }
    }

	// This [Command] code is called on the Client …
    // … but it is run on the Server!
	[Command]
    void CmdFire()
    {
        // Create the Bullet from the Bullet Prefab
        var bullet = (GameObject)Instantiate(bulletPrefab, bulletSpawn.position, bulletSpawn.rotation);

        // Add velocity to the bullet
        bullet.GetComponent<Rigidbody>().velocity = bullet.transform.forward * 6;
		
		// Spawn the bullet on the Clients
        NetworkServer.Spawn(bullet);

        // Destroy the bullet after 2 seconds
        Destroy(bullet, 2.0f);        
    }

    public override void OnStartLocalPlayer ()
    {
        GetComponent<MeshRenderer>().material.color = Color.blue;
    }
}
