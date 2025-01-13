import { Component, OnInit } from '@angular/core';
import { RouterOutlet } from '@angular/router';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
  title = 'algo-trader-gui';
  public message = "";

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch('http://0.0.0.0:5000/hello')
      .then(response => {
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.text();
        //return response.json();
      })
      .then(data => {
        this.message = data;
        console.log('API Response:', this.message);
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }
}
