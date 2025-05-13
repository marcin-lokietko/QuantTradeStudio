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

  constructor(private router: Router) { }

  ngOnInit(): void {
  }

  navigateToAssets(): void {
    this.router.navigate(['/account/assets']);
  }

  navigateToOrders(): void {
    this.router.navigate(['/account/orders']);
  }

  navigateToBots(): void {
    this.router.navigate(['/bots/launch']);
  }
}
