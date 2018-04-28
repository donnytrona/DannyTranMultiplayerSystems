using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.Networking;
using UnityEngine.UI;

public class CTFGameManager : NetworkBehaviour {

    public int m_numPlayers = 2;
    [SyncVar]
    public float m_gameTime = 60.0f;

    [SyncVar]
    public float powerUpCount = 0.0f;

    [SyncVar]
    public bool gameOver = false;

    public GameObject m_flag = null;
    public GameObject powerUpPrefab = null;

    public Text t_gameTime;
    public GameObject GameOver;
    public Text t_winner;
    public Text t_highscore;

    private string playerName = "";
    private int highscore = 0;

    public enum CTF_GameState
    {
        GS_WaitingForPlayers,
        GS_Ready,
        GS_InGame,
        GS_EndGame
    }

    [SyncVar]
    CTF_GameState m_gameState = CTF_GameState.GS_WaitingForPlayers;

    public bool SpawnFlag()
    {
        GameObject flag = Instantiate(m_flag, new Vector3(0, 3, 8.34f), new Quaternion());
        NetworkServer.Spawn(flag);
        return true;
    }

    public bool IsNumPlayersReached()
    {
        return NetworkManager.singleton.numPlayers == m_numPlayers;
    }

	// Use this for initialization
	void Start () {
    }
	
	// Update is called once per frame
	void Update ()
    {
        t_gameTime.text = m_gameTime.ToString();

	    if(isServer)
        {
            if (m_gameState == CTF_GameState.GS_WaitingForPlayers && IsNumPlayersReached())
            {
                m_gameState = CTF_GameState.GS_Ready;
            }
        }

        GameOver.SetActive(gameOver);

        UpdateGameState();
        UpdateInGame();
        UpdateEndGame();
    }

    public void SpawnPowerUp()
    {
        if(isServer)
        {
            if (powerUpCount <= 1)
            {
                powerUpCount++;

                var spawnPosition = new Vector3(
                    Random.Range(-18.0f, 18.0f),
                    0.5f,
                    Random.Range(-6.0f, 18.0f));

                var spawnRotation = Quaternion.Euler(
                    0.0f,
                    0.0f,
                    0.0f);

                var powerUp = (GameObject)Instantiate(powerUpPrefab, spawnPosition, spawnRotation);
                NetworkServer.Spawn(powerUp);
            }
        }
    }

    public void UpdateGameState()
    {
        if (m_gameState == CTF_GameState.GS_Ready)
        {
            //call whatever needs to be called
            if (isServer)
            {
                SpawnPowerUp();

                SpawnFlag();
                //change state to ingame
                m_gameState = CTF_GameState.GS_InGame;
            }
        }
    }

    public void UpdateInGame()
    {
        if (m_gameState == CTF_GameState.GS_InGame)
        {
            if (isServer)
            {
                SpawnPowerUp();

                m_gameTime -= Time.deltaTime;

                if (m_gameTime <= 0)
                {
                    m_gameTime = 0;
                    m_gameState = CTF_GameState.GS_EndGame;
                }
            }
        }
    }

    public void UpdateEndGame()
    {
        if (m_gameState == CTF_GameState.GS_EndGame)
        {
            gameOver = true;

            GameObject[] players = GameObject.FindGameObjectsWithTag("Player");

            foreach (GameObject p in players)
            {
                if (p.GetComponent<Score>().m_score > highscore)
                {
                    playerName = "Winner: Player Number - " + p.GetComponent<PlayerController>().id;
                    highscore = p.GetComponent<Score>().m_score;
                }
                p.GetComponent<PlayerController>().frozen = true;
            }

            GameObject flag = GameObject.FindGameObjectWithTag("Flag");
            flag.GetComponent<Flag>().frozen = true;

            t_winner.text = playerName;
            t_highscore.text = "Highest Score = " + highscore;
        }
    }

    public void PlayAgain()
    {
        if(isServer)
        {
            gameOver = false;

            NetworkServer.Destroy(GameObject.FindGameObjectWithTag("Flag"));

            highscore = 0;
            m_gameTime = 60.0f;
            m_gameState = CTF_GameState.GS_WaitingForPlayers;

            GameObject[] players = GameObject.FindGameObjectsWithTag("Player");

            foreach (GameObject p in players)
            {
                p.GetComponent<PlayerController>().frozen = false;
                p.GetComponent<Score>().m_score = 0;
                p.GetComponent<Health>().currentHealth = 100;
                p.GetComponent<Health>().RpcRespawn();
            }
        }
    }
}
