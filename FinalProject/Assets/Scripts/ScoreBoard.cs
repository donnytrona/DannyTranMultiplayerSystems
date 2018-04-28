using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ScoreBoard : MonoBehaviour {

    public Text[] playerScore;

    public CTFGameManager gameManager;
    public GameObject[] players;
    // Use this for initialization
    void Start () {
        
    }
	
	// Update is called once per frame
	void Update () {
        players = GameObject.FindGameObjectsWithTag("Player");

        foreach (GameObject p in players)
        {
            playerScore[p.GetComponent<PlayerController>().id - 1].text = "Score " + p.GetComponent<PlayerController>().id + " = " + p.GetComponent<Score>().m_score;
        }
        
	}
}
