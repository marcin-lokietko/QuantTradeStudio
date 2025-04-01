import { Component, OnInit } from '@angular/core';
import { RouterOutlet } from '@angular/router';
import { MatButtonModule } from '@angular/material/button';
import { MatMenuModule } from '@angular/material/menu';
import { MatIconModule } from '@angular/material/icon';
import { Router } from '@angular/router';

@Component({
  selector: 'app-root',
  imports: [RouterOutlet, MatButtonModule, MatMenuModule, MatIconModule],
  templateUrl: './app.component.html',
  styleUrl: './app.component.scss'
})
export class AppComponent {
  title = 'algo-trader-gui';

  constructor(private router: Router) {}

  ngOnInit(): void {
    this.getData();
  }

  getData(): void {
    fetch('http://localhost:5000/hello')
      .then(response => {
        if (!response.ok) {
          throw new Error(`HTTP error! status: ${response.status}`);
        }
        return response.text();
        //return response.json();
      })
      .then(data => {
        console.log('API Response:', data);
      })
      .catch(error => {
        console.error('Error fetching data:', error);
      });
  }

  navigateToAssets(): void {
    this.router.navigate(['/account/assets']);
  }
}
